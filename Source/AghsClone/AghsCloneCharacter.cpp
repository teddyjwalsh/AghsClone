// Copyright Epic Games, Inc. All Rights Reserved.

#include "AghsCloneCharacter.h"

#include <string>

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Misc/DateTime.h"
#include "Kismet/KismetMathLibrary.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "BallDropAbility.h"
#include "ShockwaveAbility.h"
#include "AIController.h"

AAghsCloneCharacter::AAghsCloneCharacter():
	MaxHealth(100.0),
	HealthRegeneration(0.1),
	Armor(0),
	MagicResist(0.25),
	IsSpellImmune(false),
	IsAttackImmune(false),
	MaxMana(100.0)
{
	Health = MaxHealth;
	Mana = MaxMana;
	//bReplicates = true;
	//SetReplicateMovement(true);
	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1600.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	//CameraBoom->SetWorldLocation(FVector(1000, 0, 0));
	//CameraBoom->AddWorldOffset(FVector(1000, 1000, 0));

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	auto new_ab = CreateDefaultSubobject<UBallDropAbility>((std::string("Ability") + std::to_string(0)).c_str());
	Abilities.push_back(new_ab);

	auto new_ab3 = CreateDefaultSubobject<UShockwaveAbility>((std::string("Ability") + std::to_string(1)).c_str());
	Abilities.push_back(new_ab3);
	for (int i = 2; i < 4; ++i)
	{
		auto new_ab2 = CreateDefaultSubobject<UAbility>((std::string("Ability") + std::to_string(i)).c_str());
		Abilities.push_back(new_ab2);
	}
	auto ai_cont = Cast<AAIController>(GetController());
	//Abilities[0]->bGroundTargeted = true;
	Abilities[2]->bUnitTargeted = true;
	for (auto& ab : Abilities)
	{
		//ab->SetIsReplicated(true);
	}

	GetCapsuleComponent()->SetIsReplicated(true);
	FDetachmentTransformRules test_det(EDetachmentRule::KeepRelative, true);
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Skellie(TEXT("SkeletalMesh'/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	GetMesh()->SetSkeletalMesh(Skellie.Object);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -95));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	static ConstructorHelpers::FObjectFinder<UClass> Skellie_anim(TEXT("/Game/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C"));
	//GetMesh()->SetAnimation(Cast<UAnimationAsset>(Skellie_anim.Object));
	GetMesh()->SetAnimInstanceClass(Skellie_anim.Object);
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> ScreenMat(TEXT("SkeletalMesh'/Game/StarterContent/Materials/M_Glass.M_Glass'"));
	//GetMesh()->SetSkeletalMesh()
	//GetMesh()->SetIsReplicated(true);
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCharacterMovement()->RotationRate.Yaw = 180;
}

void AAghsCloneCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	FDateTime t = FDateTime::UtcNow();
	double ts = t.ToUnixTimestamp() + t.GetMillisecond() * 1.0 / 1000;

	if (GetLocalRole() == ROLE_Authority)
	{
		if (current_command.command_type != NONE)
		{
			CommandStateMachine(DeltaSeconds);
		}
		else if (!command_queue.IsEmpty())
		{
			NextCommand();
		}
	}

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			int32 active_ability = GetTargetingActive();
			if (active_ability >= 0)
			{
				FHitResult TraceHitResult;
				PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
				FVector CursorFV = TraceHitResult.ImpactNormal;
				FRotator CursorR = CursorFV.Rotation();
				Abilities[active_ability]->TargetingDecal->SetWorldLocation(TraceHitResult.Location);
				Abilities[active_ability]->TargetingDecal->SetWorldRotation(CursorR);
			}
			else
			{
				FHitResult TraceHitResult;
				PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
				FVector CursorFV = TraceHitResult.ImpactNormal;
				FRotator CursorR = CursorFV.Rotation();
				CursorToWorld->SetWorldLocation(TraceHitResult.Location);
				CursorToWorld->SetWorldRotation(CursorR);
			}
		}
	}
}

void AAghsCloneCharacter::CommandStateMachine(float dt)
{
	static float MoveTolerance = 10.0;

	switch (current_command.command_type)
	{
		case MOVE:
		{
			auto my_loc3 = GetActorLocation();
			auto my_loc = FVector2D(my_loc3);
			if ((my_loc - FVector2D(current_command.location)).Size() < MoveTolerance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Completed Move"));
				NextCommand();
			}
			else if (current_destination != current_command.location)
			{
				current_destination = current_command.location;
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), current_destination);

			}
			break;
		}
		case ATTACK_MOVE:
		{
			break;
		}
		case ABILITY:
		{
			auto forward_vec = (GetActorForwardVector());
			auto my_loc3 = GetActorLocation();
			auto my_loc = FVector2D(my_loc3);

			auto ability_vec = ((current_command.location) - my_loc3);
			ability_vec.Z = 0;
			ability_vec.Normalize();
			float angle_diff = acos(FVector::DotProduct(forward_vec, ability_vec)) * 180 / PI;
			if ((my_loc - FVector2D(current_command.location)).Size() > Abilities[current_command.ability_num]->CastRange)
			{
				current_destination = current_command.location;
				
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), current_destination);
				
			}
			else if (angle_diff > 15.0)
			{
				auto char_rot = UKismetMathLibrary::FindLookAtRotation(forward_vec, ability_vec);
				ability_vec.Z = 0;
				char_rot = FRotationMatrix::MakeFromX(ability_vec).Rotator();
				auto deg_between = char_rot.GetManhattanDistance(GetActorRotation());
				float turn_rate = 180.0;
				float inc = dt*1/(deg_between / turn_rate);
				char_rot = UKismetMathLibrary::RLerp(GetActorRotation(), char_rot, inc, true);
				char_rot.Normalize();
				SetActorRelativeRotation(char_rot);
			}
			else
			{
				if (TriggerTargetedAbility(current_command.ability_num, current_command.location))
				{
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
					NextCommand();
				}
			}
			break;
		}
		default:
			break;
	}
}

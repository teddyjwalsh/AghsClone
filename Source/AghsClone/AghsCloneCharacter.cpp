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
#include "Components/PointLightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/NetworkObjectList.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "BallDropAbility.h"
#include "ShockwaveAbility.h"
#include "InventoryComponent.h"
#include "AIController.h"

AAghsCloneCharacter::AAghsCloneCharacter() :
	MaxHealth(100.0),
	HealthRegen(1.0),
	Armor(0),
	MagicResist(0.25),
	IsSpellImmune(false),
	IsAttackImmune(false),
	MaxMana(100.0),
	ManaRegen(1.0),
	attack_damage(20),
	vision_radius(1000),
	AttackSpeed(0),
	InitialAttackSpeed(100),
	BaseMovespeed(300),
	BaseAttackTime(1.7)
{
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	Health = 1.0;
	Mana = 1.0;
	attack_range = 600;
	//bReplicates = true;
	SetReplicates(true);
	bReplicates = true;
	SetReplicateMovement(true);

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetIsReplicated(false);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 300;

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

	GetMesh()->SetLightingChannels(false, false, true);

	VisionLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("VisionLight"));
	VisionLight->SetupAttachment(RootComponent);
	VisionLight->SetLightingChannels(true, true, false);
	VisionLight->SetLightFalloffExponent(0.1);
	VisionLight->SetSpecularScale(0);
	VisionLight->ShadowResolutionScale = 8.0;
	VisionLight->bUseInverseSquaredFalloff = false;
	VisionLight->SetAttenuationRadius(GetVisionRadius());
	VisionLight->SetIntensity(20);

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);
	Wallet = CreateDefaultSubobject<UWalletComponent>(TEXT("Wallet"));
	Wallet->SetIsReplicated(true);
	Bounty = CreateDefaultSubobject<UBountyComponent>(TEXT("Bounty"));
	Bounty->SetIsReplicated(true);

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
	CursorToWorld->SetRelativeLocation(FVector(0, 0, -90));

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	auto new_ab = CreateDefaultSubobject<UBallDropAbility>((std::string("Ability") + std::to_string(0)).c_str());
	Abilities.Add(new_ab);

	auto new_ab3 = CreateDefaultSubobject<UShockwaveAbility>((std::string("Ability") + std::to_string(1)).c_str());
	new_ab3->SetMaterial("shockwave");
	Abilities.Add(new_ab3);
	for (int i = 2; i < 4; ++i)
	{
		auto new_ab2 = CreateDefaultSubobject<UAbility>((std::string("Ability") + std::to_string(i)).c_str());
		Abilities.Add(new_ab2);
	}
	auto ai_cont = Cast<AAIController>(GetController());
	//Abilities[0]->bGroundTargeted = true;
	Abilities[2]->bUnitTargeted = true;
	for (auto& ab : Abilities)
	{
		ab->SetIsReplicated(true);
	}

	//GetCapsuleComponent()->SetIsReplicated(true);
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
	GetCharacterMovement()->RotationRate.Yaw = 1146;
	GetCapsuleComponent()->SetIsReplicated(true);
	GetMesh()->SetIsReplicated(true);
}

void AAghsCloneCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	ApplyHealthRegen(DeltaSeconds);
	ApplyManaRegen(DeltaSeconds);

	//UStaticMesh::Array
	GetCharacterMovement()->MaxWalkSpeed = GetMovespeed();

	if (!GetWorld()->IsServer())
	{
		//GetCapsuleComponent()->SetVisibility(false, true);
	}

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
	else
	{
		auto pc = GetWorld()->GetFirstPlayerController();
		auto nc = pc->GetNetConnection();
		auto nd = pc->GetNetDriver();
		auto& nol = nd->GetNetworkObjectList();
		//FlushNetDormancy();

		if (NetDormancy != DORM_Awake)
		{
			//GetMesh()->SetVisibility(false);
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
				//CursorToWorld->SetWorldLocation(TraceHitResult.Location);
				//CursorToWorld->SetWorldRotation(CursorR);
			}
		}
	}
}

void AAghsCloneCharacter::ProcessAbilityCommand(const FCommand& in_command, float dt)
{
	auto forward_vec = (GetActorForwardVector());
	auto my_loc3 = GetActorLocation();
	auto my_loc = FVector2D(my_loc3);

	auto ability_vec = ((in_command.location) - my_loc3);
	ability_vec.Z = 0;
	ability_vec.Normalize();
	float angle_diff = acos(FVector::DotProduct(forward_vec, ability_vec)) * 180 / PI;
	float turn_rate = 1146.0;
	if ((my_loc - FVector2D(in_command.location)).Size() > GetAbility(in_command.ability_num)->GetCastRange())
	{
		current_destination = in_command.location;

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), current_destination);

	}
	else if (angle_diff > 15.0)
	{
		auto char_rot = UKismetMathLibrary::FindLookAtRotation(forward_vec, ability_vec);
		ability_vec.Z = 0;
		char_rot = FRotationMatrix::MakeFromX(ability_vec).Rotator();
		auto char_rotator = char_rot.GetEquivalentRotator();
		char_rotator.Pitch = 0;
		char_rotator.Roll = 0;
		auto diff_rot = char_rot - GetActorRotation();
		auto deg_between = char_rot.GetManhattanDistance(GetActorRotation());

		float inc = dt * turn_rate;
		if (deg_between < inc)
		{
			inc = deg_between;
		}
		//char_rot = UKismetMathLibrary::RLerp(GetActorRotation(), char_rot, inc, true);
		//char_rot.Normalize();
		AddActorLocalRotation(FRotator(0, inc, 0));
		//SetActorRelativeRotation(char_rot);
	}
	else
	{
		if (TriggerTargetedAbility(current_command.ability_num, current_command.location))
		{
			my_loc3 = GetActorLocation();
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
			NextCommand();
		}
	}
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void AAghsCloneCharacter::ProcessAttackMove(const FCommand& in_command, float dt)
{
	auto forward_vec = (GetActorForwardVector());
	auto my_loc3 = GetActorLocation();
	auto my_loc = FVector2D(my_loc3);

	auto target_2d = FVector2D(in_command.target->GetActorLocation());

	auto ability_vec = ((in_command.target->GetActorLocation()) - my_loc3);
	ability_vec.Z = 0;
	ability_vec.Normalize();
	float angle_diff = acos(FVector::DotProduct(forward_vec, ability_vec)) * 180 / PI;
	float turn_rate = 1146.0;
	if (FVector::DotProduct(FVector::CrossProduct(forward_vec, ability_vec), FVector(0, 0, 1)) < 0)
	{
		turn_rate = -turn_rate;
	}
	if (in_command.target == this || !IsValid(in_command.target))
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
		NextCommand();
	}
	if ((my_loc - target_2d).Size() > attack_range)
	{
		current_destination = in_command.location;
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), in_command.target);
	}
	else if (angle_diff > 15.0)
	{
		auto char_rot = UKismetMathLibrary::FindLookAtRotation(forward_vec, ability_vec);
		ability_vec.Z = 0;
		char_rot = FRotationMatrix::MakeFromX(ability_vec).Rotator();
		auto char_rotator = char_rot.GetEquivalentRotator();
		char_rotator.Pitch = 0;
		char_rotator.Roll = 0;
		auto diff_rot = char_rot - GetActorRotation();
		auto deg_between = char_rot.GetManhattanDistance(GetActorRotation());
		
		float inc = dt * turn_rate;
		if (deg_between < inc)
		{
			inc = deg_between;
		}
		//char_rot = UKismetMathLibrary::RLerp(GetActorRotation(), char_rot, inc, true);
		//char_rot.Normalize();
		AddActorLocalRotation(FRotator(0, inc, 0));
		//SetActorRelativeRotation(char_rot);
	}
	else
	{
		if (AttackActor(in_command.target))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
			//NextCommand();
		}
	}
}

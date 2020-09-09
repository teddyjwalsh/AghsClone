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
	AttackSpeed(100),
	BaseMovespeed(300),
	BaseAttackTime(1.7),
	char_state_time(0.0)
{
	for (int i = START_STAT_TYPE; i != END_STAT_TYPE; ++i)
	{
		stats.Add(nullptr);
	}

	AddStat(StatMaxHealth, &MaxHealth);
	AddStat(StatMaxMana, &MaxMana);
	AddStat(StatArmor, &Armor);
	AddStat(StatAttackSpeed, &AttackSpeed);
	AddStat(StatMovespeed, &BaseMovespeed);
	AddStat(StatAttackDamage, &attack_damage);
	AddStat(StatMagicResist, &MagicResist);
	AddStat(StatHealthRegen, &HealthRegen);
	AddStat(StatManaRegen, &ManaRegen);

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
	StatInterfaces.Add(Cast<IStatInterface>(Inventory));

	Wallet = CreateDefaultSubobject<UWalletComponent>(TEXT("Wallet"));
	Wallet->SetIsReplicated(true);

	Bounty = CreateDefaultSubobject<UBountyComponent>(TEXT("Bounty"));
	Bounty->SetIsReplicated(true);

	temp_sphere = CreateDefaultSubobject<USphereComponent>(TEXT("TempSphere"));
	//temp_sphere->SetAbsolute(true, true, true);
	temp_sphere->SetupAttachment(GetRootComponent());
	temp_sphere->SetSphereRadius(0.1, true);
	temp_sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatusManager = CreateDefaultSubobject<UStatusManager>(TEXT("StatusManager"));
	StatusManager->SetIsReplicated(true);
	StatInterfaces.Add(Cast<IStatInterface>(StatusManager));

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

	//static ConstructorHelpers::FObjectFinder<UClass> Skellie_anim(TEXT("/Game/Mannequin/Animations/ThirdPerson_AnimBP.ThirdPerson_AnimBP_C"));
	static ConstructorHelpers::FObjectFinder<UClass> Skellie_anim(TEXT("/Game/Animations/AghsCharAnimBP.AghsCharAnimBP_C"));
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

	float attack_point = GetAttackPoint();
	float backswing = GetAttackBackswing();

	//UStaticMesh::Array
	GetCharacterMovement()->MaxWalkSpeed = GetMovespeed();

	if (!GetWorld()->IsServer())
	{
		//GetCapsuleComponent()->SetVisibility(false, true);
	}

	FDateTime t = FDateTime::UtcNow();
	double ts = t.ToUnixTimestamp() + t.GetMillisecond() * 1.0 / 1000;

	if (HasAuthority())
	{
		if (ChanneledAbility)
		{
			if (ChanneledAbility->IsDoneChanneling())
			{
				ChanneledAbility->EndChannel();
				ChanneledAbility = nullptr;
				NextCommand();
			}
		}
        if (ChanneledAbility && GetLastCommand() != GetCurrentCommand())
        {
            ChanneledAbility->EndChannel();
            ChanneledAbility = nullptr;
            NextCommand();
        }
		if (current_command.command_type != ATTACK_MOVE)
		{
			char_state_time = 0.0;
		}
		if (StatusManager->GetStunned())
		{
			char_state = Stunned;
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
            if (ChanneledAbility)
            {
                ChanneledAbility->EndChannel();
                ChanneledAbility = nullptr;
                NextCommand();
            }
		}
		else if (current_command.command_type != NONE)
		{
			if (!(StatusManager->GetSilenced() && current_command.command_type == ABILITY))
			{
				CommandStateMachine(DeltaSeconds);
			}
			if (StatusManager->GetRooted())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
			}
		}
		else if (!command_queue.IsEmpty())
		{
			NextCommand();
		}
		else
		{
			char_state = Idle;
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

void AAghsCloneCharacter::ProcessAbilityCommand(const FCommand& in_command, float dt, bool is_new)
{
	auto ability_interface = GetAbility(in_command.ability_num);
	// Establish location of command
	FVector command_loc = in_command.location;
	if (in_command.unit_targeted)
	{
		command_loc = in_command.target->GetActorLocation();
	}
	
	if (is_new)
	{
		char_state = MovementTowards;
		if (!ability_interface->IsGroundTargeted() && !ability_interface->IsUnitTargeted())
		{
			char_state = CastPoint;
		}
	}

	FVector diff_vector = command_loc - GetActorLocation();

	FDateTime t = FDateTime::UtcNow();
	double ts = t.ToUnixTimestamp() + t.GetMillisecond() * 1.0 / 1000;
	float last_attack_diff = ts - last_attack_time;
	FVector next_point;
	switch (char_state)
	{
	case MovementTowards:
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), command_loc);
		next_point = GetNextPathPoint(command_loc);
		FVector toward_vec = next_point - GetActorLocation();
		toward_vec.Z = 0;
		toward_vec.Normalize();
		auto FV1 = GetActorForwardVector();
		float angle_diff = acos(FVector::DotProduct(FV1, toward_vec)) * 180 / PI;
		if (angle_diff > 11.5)
		{
			PauseMove();
			float turn_rate = 500;
			float inc = dt * turn_rate;
			if (angle_diff < inc)
			{
				inc = angle_diff;
			}
			AddActorLocalRotation(FRotator(0, -inc, 0));
			auto FV2 = GetActorForwardVector();
			float angle_diff2 = acos(FVector::DotProduct(FV2, toward_vec)) * 180 / PI;
			if (angle_diff2 > angle_diff)
			{
				AddActorLocalRotation(FRotator(0, 2 * inc, 0));
			}
		}

		//SetDestination(command_loc);
		//Exit Conditions
		if (diff_vector.Size() <= ability_interface->GetCastRange() && angle_diff < 11.5)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
			CastTimer = 0.0;
			char_state = CastPoint;
		}
		break;
	}
	case CastPoint:
		CastTimer += dt;
		//Exit Conditions
		if (CastTimer > ability_interface->GetCastPoint())
		{
			if (in_command.unit_targeted)
			{
				if (TriggerTargetedAbility(in_command.ability_num, in_command.target) && !ability_interface->IsChanneling())
				{
					FVector my_loc3 = GetActorLocation();
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
					char_state = CastBackswing;
				}
			}
			else if (ability_interface->IsGroundTargeted())
			{
				if (TriggerTargetedAbility(in_command.ability_num, in_command.location) && !ability_interface->IsChanneling())
				{
					FVector my_loc3 = GetActorLocation();
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
					char_state = CastBackswing;
				}
			}
			else
			{
				if (TriggerAbility(in_command.ability_num) && !ability_interface->IsChanneling())
				{
					FVector my_loc3 = GetActorLocation();
					UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), my_loc3);
					char_state = CastBackswing;
				}
			}
			if (ability_interface->IsChanneling())
			{
				ChanneledAbility = ability_interface;
				char_state = Channeling;
			}
		}
		break;
	case CastBackswing:
		CastTimer += dt;
		//Exit Conditions
		if (CastTimer > ability_interface->GetCastPoint() + ability_interface->GetCastBackswing())
		{
			char_state = Idle;
			NextCommand();
		}
		break;
	case Channeling:
		if (!StatusManager->GetStunned() && !StatusManager->GetSilenced())
		{
			ability_interface->TickChannel(dt);
			return;
		}
		else
		{
			ability_interface->EndChannel();
			NextCommand();
			return;
		}
		break;
	default:
		break;
	}
}

template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

void AAghsCloneCharacter::ProcessAttackMove(const FCommand& in_command, float dt, bool is_new)
{
	// Establish location of command
	FVector command_loc = in_command.location;
	if (in_command.unit_targeted)
	{
		command_loc = in_command.target->GetActorLocation();
	}
	FVector diff_vector = command_loc - GetActorLocation();

    if (is_new)
    {
        char_state = MovementTowards;
    }

    FDateTime t = FDateTime::UtcNow();
    double ts = t.ToUnixTimestamp() + t.GetMillisecond() * 1.0 / 1000;
    float last_attack_diff = ts - last_attack_time;
	FVector next_point;
	switch(char_state)
    {
        case MovementTowards:
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), command_loc);
			next_point = GetNextPathPoint(command_loc);
			FVector toward_vec = next_point - GetActorLocation();
			toward_vec.Z = 0;
			toward_vec.Normalize();
			auto FV1 = GetActorForwardVector();
			float angle_diff = acos(FVector::DotProduct(FV1, toward_vec)) * 180 / PI;
			if (angle_diff > 11.5)
			{
				PauseMove();
				float turn_rate = 500;
				float inc = dt * turn_rate;
				if (angle_diff < inc)
				{
					inc = angle_diff;
				}
				AddActorLocalRotation(FRotator(0, -inc, 0));
				auto FV2 = GetActorForwardVector();
				float angle_diff2 = acos(FVector::DotProduct(FV2, toward_vec)) * 180 / PI;
				if (angle_diff2 > angle_diff)
				{
					AddActorLocalRotation(FRotator(0, 2*inc, 0));
				}
			}

			//SetDestination(command_loc);
			//Exit Conditions
			if (diff_vector.Size() <= attack_range && angle_diff < 11.5)
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());
				AttackTimer = 0.0;
				char_state = Idle;
			}
			break;
		}
        case Idle:
            SetDestination(GetActorLocation());
            //Exit Conditions
            if (diff_vector.Size() > attack_range)
            {
                char_state = MovementTowards;
            }
            if (last_attack_diff > GetAttackTime() - GetAttackPoint())
            {
                AttackTimer = 0.0;
                char_state = AttackPoint;
            }
            break;
        case AttackPoint:
            AttackTimer += dt;
            //Exit Conditions
            if (AttackTimer > GetAttackPoint())
            {
                AttackActor(in_command.target);
                char_state = AttackBackswing; 
            }
            break;
        case AttackBackswing:
            AttackTimer += dt;
            //Exit Conditions
            if (AttackTimer > GetAttackPoint() + GetAttackBackswing())
            {
                char_state = Idle; 
            }
            break;
        default:
            break; 
    }
}

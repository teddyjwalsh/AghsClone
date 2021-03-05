// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <array>
#include <cassert>

#include "Math/UnrealMathUtility.h"
#include "CoreMinimal.h"
#include "Hero.h"
#include "AttackProjectile.h"
#include "SniperHero.generated.h"

UCLASS()
class AGHSCLONE_API AAssassinateProjectile : public AAttackProjectile
{
	GENERATED_BODY()

public:
	float damage;

	AAssassinateProjectile()
	{
		auto mat = new ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/characters/wizard/AssassinateProjMat.AssassinateProjMat'"));
		model->SetMaterial(0, mat->Object);
	}

	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult) override
	{
		if (HasAuthority() && OtherActor == GetTarget())
		{

			auto health_comp = Cast<IHealthInterface>(OtherActor);
			if (health_comp)
			{
				DamageInstance assassinate_damage;
				assassinate_damage.damage_type = MagicDamage;
				assassinate_damage.is_attack = false;
				assassinate_damage.value = damage;
				health_comp->ApplyDamage(assassinate_damage, GetOwner());
			}
			Destroy();
		}
	}

};

UCLASS()
class AGHSCLONE_API UAssassinateAbility : public UAbility
{
	GENERATED_BODY()
public:
	UAssassinateAbility()
	{
		CastPoint = 2.0;
		CastBackswing = 1.37;
		bUnitTargeted = true;
		max_level = 3;
		CastRange = 3000;
		ManaCosts = { {175,225,275} };
		Cooldowns = { {20,15,10} };
	}

	virtual void OnUnitActivation(AActor* target) override
	{
		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		/*
		auto status = GetWorld()->SpawnActor<AAssassinateProjectile>(GetOwner()->GetActorLocation(), Rotation, SpawnInfo);
		status->SetRadius(50);
		status->EnableUnitTargetedProjectile(target);
		status->SetProjectileSpeed(200);
		*/
		//status->SetActorLocation(GetOwner()->GetActorLocation());
		auto proj = GetWorld()->SpawnActor<AAssassinateProjectile>();
		proj->SetActorLocation(GetOwner()->GetActorLocation());
		proj->SetTarget(target);
		proj->SetShooter(GetOwner());
		proj->SetSpeed(2500);
		proj->SetOwner(GetOwner());
		proj->damage = 200;
	}
};

UCLASS()
class AGHSCLONE_API AHeadshotSlow : public AStatusEffect
{
	GENERATED_BODY()

public:
	float movespeed_multiplier = 1.0;

	AHeadshotSlow()
	{
		max_duration = 0.5;
		AddStatMult(StatMovespeed, &movespeed_multiplier);
	}
};

UCLASS()
class AGHSCLONE_API UHeadshotAbility : public UAbility
{
	GENERATED_BODY()

	std::vector<float> Damages = { {20,50,80,110} };
	float slow = 0.0;

public:
	UHeadshotAbility()
	{
		bOnHit = true;
		max_level = 4;
	}

	virtual void OnHit(DamageInstance& damage, AActor* unit) override
	{ 
		auto health_comp = Cast<IHealthInterface>(unit);
		UStatusManager* status_manager = Cast<UStatusManager>(unit->GetComponentByClass(UStatusManager::StaticClass()));
		int32 res = FMath::RandRange(0, 100);
		if (res < 40)
		{
			if (health_comp)
			{
				DamageInstance headshot_damage;
				headshot_damage.damage_type = PhysicalDamage;
				headshot_damage.is_attack = false;
				headshot_damage.value = (GetLevel()) ? Damages[(unsigned int)(GetLevel() - 1)] : 0.0;
				health_comp->ApplyDamage(headshot_damage, GetOwner());

				if (status_manager)
				{
					FVector Location(0.0f, 0.0f, 0.0f);
					FRotator Rotation(0.0f, 0.0f, 0.0f);
					FActorSpawnParameters SpawnInfo;
					auto status = GetWorld()->SpawnActor<AHeadshotSlow>(Location, Rotation, SpawnInfo);
					status->movespeed_multiplier = slow;
					status_manager->AddStatus(status);
				}
			}
		}
	}
};

UCLASS()
class AGHSCLONE_API AShrapnelSlow : public AStatusEffect
{
	GENERATED_BODY()

	

public:
	float damage = 0;
	float movespeed_multiplier = 1.0;
	float slow;
	float damage_tick = 1.0;

	AShrapnelSlow()
	{
		SetAura(450);
		SetTickTime(1.0);
		tick_period = 1.0;
		max_duration = 10.0;
		slow = -100;
		movespeed_multiplier = 1.0;
		//AddStat(StatMovespeed, &slow);
		AddStatMult(StatMovespeed, &movespeed_multiplier);
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.bStartWithTickEnabled = true;
		PrimaryActorTick.bAllowTickOnDedicatedServer = true;
		SetReplicates(true);
		assert(IsActorTickEnabled());
		bEnemyOnly = true;
	}

	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		printf(" SSTARTING\n");

		SetActorTickEnabled(true);
		SetActorTickInterval(0.02);
		assert(IsActorTickEnabled());
		assert(PrimaryActorTick.bCanEverTick);
	}

	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);

	}

	virtual void StatusTick()
	{
		auto health_comp = Cast<IHealthInterface>(owner);
		if (health_comp && applied_time - last_tick > damage_tick)
		{
			DamageInstance shrapnel_damage;
			shrapnel_damage.damage_type = MagicDamage;
			shrapnel_damage.is_attack = false;
			shrapnel_damage.value = damage;
			health_comp->ApplyDamage(shrapnel_damage, GetApplier());
			last_tick = applied_time;
		}
	}

	virtual void TickConnection(AActor* in_actor) override
	{

		auto hi = Cast<IHealthInterface>(in_actor);
		if (hi)
		{
			DamageInstance di;
			di.damage_type = MagicDamage;
			di.value = damage;
			di.is_attack = false;
			hi->ApplyDamage(di, GetOwner());
		}
	}
};

UCLASS()
class AGHSCLONE_API AShrapnel : public AAbilityInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* bounds;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float radius;
	bool ticked_once;

	/** A decal that projects to the cursor location. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* IceCircle;
	ConstructorHelpers::FObjectFinder<UMaterial>* DecalMaterialAsset;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* SmokeEmitter;
	UMaterialInstanceDynamic* DynDecalMaterial;
	ConstructorHelpers::FObjectFinder<UParticleSystem>* EmitterAsset;
	float anim_time;
	float duration = 10.0;
	float shrapnel_radius = 450.0;
	
	AShrapnelSlow* status;

public:
	float slow_multiplier = 1.0;

	// Sets default values for this actor's properties
	AShrapnel()
	{
		SetReplicates(true);
		PrimaryActorTick.bCanEverTick = true;
		bounds = CreateDefaultSubobject<UCapsuleComponent>("Bounds");
		SetRootComponent(bounds);
		SetOverlapper(bounds);
		SetTickHitEnabled(true);
		SetEnabled(true);
		//bounds->OnComponentBeginOverlap.AddDynamic(this, &ACrystalNova::OnTouch);
		
		SetReplicates(true);
		bounds->SetCapsuleSize(1000, 1000);
		SetStatusEffect(AShrapnelSlow::StaticClass());
		SetHitDamage(130);
		SetDamageType(MagicDamage);
		bounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ticked_once = false;

		IceCircle = CreateDefaultSubobject<UDecalComponent>(TEXT("IceCircleDecal"));
		IceCircle->SetupAttachment(RootComponent);
		DecalMaterialAsset = new ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Materials/IceCircleDecal.IceCircleDecal'"));
		EmitterAsset = new ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("ParticleSystem'/Game/Effects/CrystalNovaSmoke.CrystalNovaSmoke'"));
		SmokeEmitter = NewObject<UParticleSystemComponent>(this, FName("IceSmoke"));
		SmokeEmitter->SetupAttachment(RootComponent);
		//SetResetHitOnTick(true);
	}

	void SetRadius(float in_radius)
	{
		radius = in_radius;
		bounds->SetCapsuleRadius(radius);
		//SmokeEmitter->SetFloatParameter(FName("StartRadius"), radius);
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		SetLifeSpan(duration);
		//status = NewObject<AShrapnelSlow>(GetWorld(), AShrapnelSlow::StaticClass());
		DynDecalMaterial = UMaterialInstanceDynamic::Create(DecalMaterialAsset->Object, this);
		anim_time = 0;
		if (DecalMaterialAsset->Succeeded())
		{
			IceCircle->SetDecalMaterial(DynDecalMaterial);
		}
		IceCircle->DecalSize = FVector(100.0f, 500.0f, 500.0f);
		IceCircle->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		IceCircle->SetRelativeLocation(FVector(0, 0, 0));
		IceCircle->SetSortOrder(0);
		//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterAsset->Object->Get, GetActorLocation(), FRotator(), true);
		//SetActorTickInterval(1.0);
	}

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);
		
		if (!ticked_once)
		{
			//SmokeEmitter = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("IceSmoke"));
			SmokeEmitter->SetTemplate(EmitterAsset->Object);
			SmokeEmitter->SetFloatParameter(FName("StartRadius"), float(int(radius)));

			SmokeEmitter->SetRelativeLocation(FVector(0, 0, 0));

			SmokeEmitter->SetIsReplicated(true);
			AddOwnedComponent(SmokeEmitter);
		}
		ticked_once = true;
		//SetEnabled(false);
		anim_time += DeltaTime;
		DynDecalMaterial->SetScalarParameterValue("AnimTime", anim_time);
		//DrawDebugComponents();
		//Destroy();
	}

	UFUNCTION()
		void OnTouch(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 other_body_index, bool bFromSweep, const FHitResult& hit_res)
	{

	}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AShrapnel, IceCircle);
		DOREPLIFETIME(AShrapnel, SmokeEmitter);
		DOREPLIFETIME(AShrapnel, radius);
	}

};


UCLASS()
class AGHSCLONE_API UShrapnelAbility : public UAbility
{
	GENERATED_BODY()

		std::vector<float> Slows;

	UShrapnelAbility()
	{
		bGroundTargeted = true;
		CastRange = 700;
		ManaCost = 130;
		Cooldown = 35;
		ManaCosts = { {50,50,50,50} };
		Cooldowns = { {11, 10, 9, 8 } };
		Damages = { {20, 35, 50, 65 } };
		Slows = { {0.88, 0.82, 0.76, 0.70 } };
		max_level = 4;
		max_charges = 3;
		bIsCharges = true;
		charges = 3;
		current_level = 0;
		radius = 450;
		CastPoint = 0.3;
		CastBackswing = 0.0;
		SetStatusEffect(AShrapnelSlow::StaticClass());
	}

	std::vector<AShrapnel*> instances;
	std::vector<AShrapnelSlow*> slow_instances;
	float radius;

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
	}

public:

	// Called every frame
	virtual void OnGroundActivation(const FVector& target) override
	{
		if (GetOwner()->HasAuthority())
		{
			//auto new_instance = NewObject<UBallDrop>(this);
			auto new_instance = GetWorld()->SpawnActor<AShrapnel>();
			new_instance->SetActorLocation(target + FVector(0, 0, 10));
			new_instance->SetOwner(GetOwner());
			new_instance->SetRadius(radius);
			new_instance->SetHitDamage(GetDamage());
			new_instance->slow_multiplier = (GetLevel()) ? Slows[(unsigned int)(GetLevel() - 1)] : 1.0;
			auto aghs_char = Cast<AAghsCloneCharacter>(GetOwner());
			if (aghs_char)
			{
				new_instance->team = aghs_char->GetTeam();
			}
			//new_instance->SetActorTickInterval(TickInterval);
			//new_instance->SetPos(target + FVector(0, 0, 200));
			instances.push_back(new_instance);
			auto new_status = GetWorld()->SpawnActor<AShrapnelSlow>();
			new_status->movespeed_multiplier = (GetLevel()) ? Slows[(unsigned int)(GetLevel() - 1)] : 1.0;
			new_status->damage = (GetLevel()) ? Damages[(unsigned int)(GetLevel() - 1)] : 1.0;
			new_status->SetOwner(new_instance);
			new_status->SetApplier(GetOwner());
			new_status->SetActorLocation(target + FVector(0, 0, 10));
			slow_instances.push_back(new_status);
		}
	}
};

UCLASS(Blueprintable)
class ATakeAimStatus : public AStatusEffect
{
	GENERATED_BODY()

public:
	float AttackRange;
	ATakeAimStatus()
	{
		max_duration = 20.0;
		AttackRange = 0.5;
		AddStat(StatAttackRange, &AttackRange);
		bIsAura = false;
	}
};

UCLASS()
class AGHSCLONE_API UTakeAimAbility : public UAbility
{
	GENERATED_BODY()

	std::vector<float> AttackRanges = { {100,200,300,400} };

	UPROPERTY()
	ATakeAimStatus* status;

	UTakeAimAbility()
	{
		//aura = NewObject<UArcaneAura>();
		max_level = 4;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		status = NewObject<ATakeAimStatus>(GetWorld(), ATakeAimStatus::StaticClass());
	}

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		if (GetWorld()->GetTimeSeconds() < 2)
		{
			return;
		}
		if (GetLevel())
		{
			status->AttackRange = AttackRanges[(unsigned int)GetLevel()];
		}
		auto owner = Cast<AAghsCloneCharacter>(GetOwner());
		if (owner)
		{
			UStatusManager* status_manager = Cast<UStatusManager>(owner->GetComponentByClass(UStatusManager::StaticClass()));

			if (status_manager)
			{
				status->AttackRange = AttackRanges[GetLevel()];
				status_manager->AddStatus(status);
			}
		}

	}
};


/**
 * 
 */
UCLASS()
class AGHSCLONE_API ASniperHero : public AHero
{
	GENERATED_BODY()

public:
	ASniperHero()
	{
		SetBaseMovespeed(285);
		SetMaxHealth(200);
		SetMaxMana(75);
		SetManaRegen(0.0);
		SetHealthRegen(0.0);
		SetArmor(-1);
		SetAttackDamage(31);
		SetAttackProjectileSpeed(3000);
		SetArmor(-1);
		SetAttackRange(550);
		BaseAttackPoint = 0.17;
		BaseAttackBackswing = 0.7;
		//attributes->SetPrimaryAttribute(AttrAgility);
		attributes->BaseStrength = 19;
		attributes->StrengthGain = 2.0;
		attributes->BaseAgility = 27;
		attributes->AgilityGain = 3.2;
		attributes->BaseIntelligence = 15;
		attributes->IntelligenceGain = 2.6;
		
		Abilities.Empty();
		auto new_ab3 = CreateDefaultSubobject<UShrapnelAbility>((std::string("Ability") + std::to_string(21)).c_str());
		new_ab3->SetMaterial("shockwave");
		Abilities.Add(new_ab3);

		auto new_ab2 = CreateDefaultSubobject<UHeadshotAbility>((std::string("Ability") + std::to_string(22)).c_str());
		Abilities.Add(new_ab2);

		auto new_ab = CreateDefaultSubobject<UTakeAimAbility>((std::string("Ability") + std::to_string(23)).c_str());
		Abilities.Add(new_ab);

		auto new_ab4 = CreateDefaultSubobject<UAssassinateAbility>((std::string("Ability") + std::to_string(24)).c_str());
		Abilities.Add(new_ab4);
		
		
		/*
		auto new_ab4 = CreateDefaultSubobject<UArcaneAuraAbility>((std::string("Ability") + std::to_string(22)).c_str());
		new_ab4->SetMaterial("shockwave");
		Abilities.Add(new_ab4);

		auto new_ab5 = CreateDefaultSubobject<UFreezingFieldAbility>((std::string("Ability") + std::to_string(23)).c_str());
		new_ab5->SetMaterial("shockwave");
		Abilities.Add(new_ab5);
		*/
		for (auto& ab : Abilities)
		{
			ab->SetIsReplicated(true);
		}
		
	}
};
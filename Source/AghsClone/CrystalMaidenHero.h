// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <array>

#include "CoreMinimal.h"
#include "EngineUtils.h"

#include "Hero.h"
#include "Components/CapsuleComponent.h"
#include "StatusManager.h"
#include "CrystalMaidenHero.generated.h"

UCLASS(Blueprintable)
class UCrystalNovaSlow : public UStatusEffect
{
	GENERATED_BODY()

	float slow;
	float movespeed_multiplier;

public:
	UCrystalNovaSlow()
	{
		
		max_duration = 4.5;
		slow = -100;
		movespeed_multiplier = 0.8;
		AddStat(StatMovespeed, &slow);
		AddStatMult(StatMovespeed, &movespeed_multiplier);
	}
};

UCLASS()
class AGHSCLONE_API ACrystalNova : public AAbilityInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* bounds;
	float radius;
	bool ticked_once;


public:
	// Sets default values for this actor's properties
	ACrystalNova()
	{
		
		PrimaryActorTick.bCanEverTick = true;
		bounds = CreateDefaultSubobject<UCapsuleComponent>("Bounds");
		SetRootComponent(bounds);
		SetOverlapper(bounds);
		SetTickHitEnabled(true);
		SetEnabled(true);
		//bounds->OnComponentBeginOverlap.AddDynamic(this, &ACrystalNova::OnTouch);
		SetReplicates(true);
		bounds->SetCapsuleSize(1000, 1000);
		SetStatusEffect(UCrystalNovaSlow::StaticClass());
		SetHitDamage(130);
		SetDamageType(MagicDamage);
		ticked_once = false;
		
	}

	void SetRadius(float in_radius)
	{
		radius = in_radius;
		bounds->SetCapsuleRadius(radius);
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		SetLifeSpan(3.0);
	}

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override
	{
		Super::Tick(DeltaTime);
		ticked_once = true;
		SetEnabled(false);
		//DrawDebugComponents();
		//Destroy();
	}

	UFUNCTION()
	void OnTouch(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 other_body_index, bool bFromSweep, const FHitResult& hit_res)
	{

	}

};


UCLASS()
class AGHSCLONE_API UCrystalNovaAbility : public UAbility
{
	GENERATED_BODY()

	UCrystalNovaAbility()
	{
		bGroundTargeted = true;
		CastRange = 700;
		ManaCost = 130;
		Cooldown = 11;
		ManaCosts = { {130, 145, 160, 175} };
		Cooldowns = { {11, 10, 9, 8 } };
		max_level = 4;
		current_level = 0;
		radius = 425;
		SetStatusEffect(UCrystalNovaSlow::StaticClass());
	}

	std::vector<ACrystalNova*> instances;
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
		//auto new_instance = NewObject<UBallDrop>(this);
		auto new_instance = GetWorld()->SpawnActor<ACrystalNova>();
		new_instance->SetActorLocation(target + FVector(0, 0, 0));
		new_instance->SetOwner(GetOwner());
		new_instance->SetRadius(radius);
		//new_instance->SetPos(target + FVector(0, 0, 200));
		instances.push_back(new_instance);
	}
};

UCLASS(Blueprintable)
class UFrostbiteRoot : public UStatusEffect
{
	GENERATED_BODY()

public:

	float time = 0;
	float last_time = -123456;
	float damage_tick = 0.7;
	float total_damage = 150;
	float max_duration = 1.5;
	int32 damage_instances = 0;
	int32 max_instances;

	UFrostbiteRoot()
	{
		bRooted = true;
		max_instances = int32(max_duration / damage_tick);
	}

	virtual void TickStatus(float dt) override
	{
		time += dt;
		if (time - last_time > damage_tick)
		{
			auto hi = Cast<IHealthInterface>(GetOwner());
			if (hi && (damage_instances < max_instances))
			{
				DamageInstance di;
				di.damage_type = MagicDamage;
				di.value = total_damage * (damage_tick / max_duration);
				di.is_attack = false;
				hi->ApplyDamage(di, GetOwner());
			}
			last_time = time;
			damage_instances += 1;
			if (damage_instances >= max_instances)
			{
				bFinished = true;
			}
		}
	}
};

UCLASS()
class AGHSCLONE_API UFrostbiteAbility : public UAbility
{
	GENERATED_BODY()

	std::array<float, 4> total_damages = { { 150,200,250,300 } };
	std::array<float, 4> durations = { { 1.5,2.0,2.5,3.0 } };
	float damage_tick = 0.7;

	UFrostbiteAbility()
	{
		bUnitTargeted = true;
		CastRange = 550;
		ManaCost = 130;
		Cooldowns = { { 9,8,7,6 } };
		ManaCosts = { { 140,145,150,155 } };
		max_level = 4;
		damage_tick = 0.7;
		current_level = 0;
		radius = 425;
		SetStatusEffect(UFrostbiteRoot::StaticClass());
	}

	std::vector<UFrostbiteAbility*> instances;
	float radius;

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
	}

public:

	// Called every frame
	virtual void OnUnitActivation(AActor* in_unit) override
	{
		auto status_manager = Cast<UStatusManager>(in_unit->GetComponentByClass(UStatusManager::StaticClass()));
		if (status_manager)
		{
			auto status_effect = NewObject<UFrostbiteRoot>(GetWorld(), UFrostbiteRoot::StaticClass());
			float damage_inc = total_damages[current_level] * (damage_tick / durations[current_level]);
			int32 max_instances = int32(durations[current_level] / damage_tick);
			status_effect->total_damage = total_damages[current_level];
			status_effect->max_instances = max_instances;
			status_manager->AddStatus(status_effect);
		}
	}
};

UCLASS(Blueprintable)
class UArcaneAura : public UStatusEffect
{
	GENERATED_BODY()

	float ManaRegen;
	float self_multiplier;
	

public:
	AActor* Applier;

	UArcaneAura()
	{
		max_duration = 20.0;
		ManaRegen = 0.5;
		AddStat(StatManaRegen, &ManaRegen);
		bIsAura = true;
	}
};

UCLASS()
class AGHSCLONE_API UArcaneAuraAbility : public UAbility
{
	GENERATED_BODY()

	UPROPERTY()
	UArcaneAura* aura;
	float range;

	UArcaneAuraAbility()
	{
		aura = NewObject<UArcaneAura>();
		max_level = 4;
		range = 12345678;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
		PrimaryComponentTick.TickInterval = 0.9f;
		aura = NewObject<UArcaneAura>(GetWorld(), UArcaneAura::StaticClass());
		aura->Applier = GetOwner();
	}

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		
		if (GetWorld()->GetTimeSeconds() < 2)
		{
			return;
		}
		for (TActorIterator<AHero> act_it(GetWorld()); act_it; ++act_it)
		{
			auto owner = Cast<AAghsCloneCharacter>(GetOwner());
			if (owner)
			{
				if (act_it->GetTeam() == owner->GetTeam())
				{
					UStatusManager* status_manager = Cast<UStatusManager>(act_it->GetComponentByClass(UStatusManager::StaticClass()));
					
					if (status_manager)
					{
						status_manager->RefreshStatus(aura);
					}
				}
			}
		}
		
	}
};

UCLASS()
class AGHSCLONE_API UFreezingFieldAbility : public UAbility
{
	GENERATED_BODY()

	std::array<float, 4> damage = { { 105,170,250 } };

	UFreezingFieldAbility()
	{
		bIsChanneled = true;
		MaxChannelTime = 10;
		Cooldowns = { { 110,100,90 } };
		ManaCosts = { { 200,400,600 } };
		max_level = 3;
		current_level = 0;
		SetStatusEffect(UFrostbiteRoot::StaticClass());
	}

	std::vector<UFreezingFieldAbility*> instances;
	float radius;

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
	}

public:

	// Called every frame
	virtual void OnActivation() override
	{
		ChannelTime = MaxChannelTime;
		bDoneChanneling = false;
	}

	virtual void TickChannel(float DeltaTime) override
	{
		ChannelTime -= DeltaTime;
		if (ChannelTime <= 0)
		{
			bDoneChanneling = true;
		}
	}

	virtual void EndChannel() override
	{

	}

	void SpawnBlast(float start_angle)
	{
		/*
		float angle = GetRandomWithin90(start_angle);
		float range = GetRandomRange();
		float x_offset = range*cos(angle);
		float y_offset = range*sin(angle);
		auto new_instance = GetWorld()->SpawnActor<AFreezingFieldBlast>(;
		*/
	}

	float GetRandomWithin90(float start_angle)
	{
		return start_angle + PI / 8.0;
	}


};


/**
 * 
 */
UCLASS()
class AGHSCLONE_API ACrystalMaidenHero : public AHero
{
	GENERATED_BODY()
	
public:
	ACrystalMaidenHero()
	{
		SetBaseMovespeed(280);
		SetMaxHealth(200);
		SetMaxMana(75);
		SetArmor(-1);
		SetAttackDamage(31);
		SetAttackProjectileSpeed(900);
        BaseAttackPoint = 2;
        BaseAttackBackswing = 3;
		attributes->SetPrimaryAttribute(AttrIntelligence);
		attributes->BaseStrength = 18;
		attributes->StrengthGain = 2.2;
		attributes->BaseAgility = 16;
		attributes->AgilityGain = 1.6;
		attributes->BaseIntelligence = 16;
		attributes->IntelligenceGain = 3.3;
		Abilities.Empty();
		auto new_ab = CreateDefaultSubobject<UCrystalNovaAbility>((std::string("Ability") + std::to_string(20)).c_str());
		Abilities.Add(new_ab);

		auto new_ab3 = CreateDefaultSubobject<UFrostbiteAbility>((std::string("Ability") + std::to_string(21)).c_str());
		new_ab3->SetMaterial("shockwave");
		Abilities.Add(new_ab3);

		auto new_ab4 = CreateDefaultSubobject<UArcaneAuraAbility>((std::string("Ability") + std::to_string(22)).c_str());
		new_ab4->SetMaterial("shockwave");
		Abilities.Add(new_ab4);

		auto new_ab5 = CreateDefaultSubobject<UFreezingFieldAbility>((std::string("Ability") + std::to_string(23)).c_str());
		new_ab5->SetMaterial("shockwave");
		Abilities.Add(new_ab5);

		for (auto& ab : Abilities)
		{
			ab->SetIsReplicated(true);
		}
	}
};

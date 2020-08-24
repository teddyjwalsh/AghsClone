// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <String>

#include "CoreMinimal.h"
#include "Hero.h"
#include "Components/CapsuleComponent.h"
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
		
		max_duration = 3;
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
		SetLifeSpan(3.0);
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
		attributes->BaseStrength = 18;
		attributes->StrengthGain = 2.2;
		attributes->BaseAgility = 16;
		attributes->AgilityGain = 1.6;
		attributes->BaseIntelligence = 16;
		attributes->IntelligenceGain = 3.3;
		Abilities.Empty();
		auto new_ab = CreateDefaultSubobject<UCrystalNovaAbility>((std::string("Ability") + std::to_string(20)).c_str());
		Abilities.Add(new_ab);

		auto new_ab3 = CreateDefaultSubobject<UCrystalNovaAbility>((std::string("Ability") + std::to_string(21)).c_str());
		new_ab3->SetMaterial("shockwave");
		Abilities.Add(new_ab3);
		for (int i = 2; i < 4; ++i)
		{
			auto new_ab2 = CreateDefaultSubobject<UAbility>((std::string("Ability") + std::to_string(20 + i)).c_str());
			Abilities.Add(new_ab2);
		}
		for (auto& ab : Abilities)
		{
			ab->SetIsReplicated(true);
		}
	}
};

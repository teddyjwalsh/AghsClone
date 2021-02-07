// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <array>

#include "CoreMinimal.h"
#include "Hero.h"
#include "SniperHero.generated.h"



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
		bIsAura = true;
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
		PrimaryComponentTick.TickInterval = 0.4f;
		status = NewObject<ATakeAimStatus>(GetWorld(), ATakeAimStatus::StaticClass());
	}

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		if (GetWorld()->GetTimeSeconds() < 2)
		{
			return;
		}

		auto owner = Cast<AAghsCloneCharacter>(GetOwner());
		if (owner)
		{
			UStatusManager* status_manager = Cast<UStatusManager>(owner->GetComponentByClass(UStatusManager::StaticClass()));

			if (status_manager)
			{
				status->AttackRange = AttackRanges[GetLevel()];
				status_manager->RefreshStatus(status);
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
		auto new_ab = CreateDefaultSubobject<UTakeAimAbility>((std::string("Ability") + std::to_string(20)).c_str());
		Abilities.Add(new_ab);
		/*
		auto new_ab3 = CreateDefaultSubobject<UFrostbiteAbility>((std::string("Ability") + std::to_string(21)).c_str());
		new_ab3->SetMaterial("shockwave");
		Abilities.Add(new_ab3);

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
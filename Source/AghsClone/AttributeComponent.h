// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatInterface.h"
#include "ExperienceInterface.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UAttributeComponent : public UActorComponent,
	public IStatInterface
{
	GENERATED_BODY()

	float BaseStrength;
	float BaseAgility;
	float BaseIntelligence;
	float StrengthGain;
	float AgilityGain;
	float IntelligenceGain;

public:	
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetStat(StatType stat_type) const
	{
		IExperienceInterface* xp = Cast<IExperienceInterface>(GetOwner());
		int32 current_level = 0;
		if (xp)
		{
			current_level = xp->GetLevel();
		}
		if (stat_type == StatStrength)
		{
			return BaseStrength + StrengthGain*current_level;
		}
		if (stat_type == StatAgility)
		{
			return BaseAgility + AgilityGain * current_level;
		}
		if (stat_type == StatIntelligence)
		{
			return BaseIntelligence + IntelligenceGain * current_level;
		}
		if (stat_type == StatMaxHealth)
		{
			return 20*(BaseStrength + StrengthGain * current_level);
		}
		if (stat_type == StatHealthRegen)
		{
			return 0.1 * (BaseStrength + StrengthGain * current_level);
		}
		if (stat_type == StatArmor)
		{
			return 0.17 * (BaseAgility + AgilityGain * current_level);
		}
		if (stat_type == StatAttackSpeed)
		{
			return (BaseAgility + AgilityGain * current_level);
		}
		if (stat_type == StatMaxMana)
		{
			return 12*(BaseIntelligence + IntelligenceGain * current_level);
		}
		if (stat_type == StatManaRegen)
		{
			return 0.05 * (BaseIntelligence + IntelligenceGain * current_level);
		}
		return 0;
	}
	virtual bool SetStat(StatType stat_type, float in_stat)
	{
		if (stat_type == StatStrength)
		{
			BaseStrength = in_stat;
			return true;
		}
		if (stat_type == StatAgility)
		{
			BaseAgility = in_stat;
			return true;
		}
		if (stat_type == StatIntelligence)
		{
			BaseIntelligence = in_stat;
			return true;
		}
		return false;
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatInterface.generated.h"

enum StatType
{
	START_STAT_TYPE,
	StatMaxHealth,
	StatHealth,
	StatMaxMana,
	StatMana,
	StatAttackSpeed,
	StatAttackDamage,
	StatAttackRange,
	StatMovespeed,
	StatArmor,
	StatMagicResist,
	StatHealthRegen,
	StatManaRegen,
    StatStrength,
    StatAgility,
    StatIntelligence,
	StatTurnRate,
	END_STAT_TYPE
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IStatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//virtual bool HasStat(StatType stat_type) const = 0;
	virtual float GetStat(StatType stat_type) const = 0;
	virtual float GetStatMult(StatType stat_type) const
	{
		return 1;
	}
	virtual bool SetStat(StatType stat_type, float in_stat) = 0;
};

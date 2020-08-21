// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StatInterface.generated.h"

enum StatType
{
	StatHealth,
	StatMana,
	StatAttackSpeed,
	StatAttackDamage,
	StatMovespeed,
	StatArmor
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

	virtual bool HasStat(StatType stat_type) const = 0;
	virtual float GetStat(StatType stat_type) const
	{
		if (!HasStat(stat_type))
		{
			return 0;
		}
		return GetStatImpl(stat_type);
	}
	virtual float GetStatImpl(StatType stat_type) const = 0;
	virtual bool SetStat(StatType stat_type, float in_stat)
	{
		if (!HasStat(stat_type))
		{
			return false;
		}
		return SetStatImpl(stat_type, in_stat);
	}
	virtual float SetStatImpl(StatType stat_type, float in_stat) = 0;
};

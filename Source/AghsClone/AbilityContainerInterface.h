// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityInterface.h"
#include "Ability.h"
#include "AbilityContainerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilityContainerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IAbilityContainerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual int32 AbilityPointsUsed() const
    {
        int32 ab_count = AbilityCount();
        int32 total_points = 0;
        bool retval = false;
        for (int i = 0; i < ab_count; ++i)
        {
            UAbility* ability = Cast<UAbility>(GetAbility(i));
            if (ability)
            {
                int32 ab_level = ability->GetLevel();
                total_points += ab_level;
            }
        }
        return total_points;
    }

    virtual bool CanLevelAbility(int32 ability_num, int32 current_level) const
    {
        int32 ab_count = AbilityCount();
        int32 total_points = AbilityPointsUsed();
        bool retval = false;
        if (total_points >= current_level)
        {
            return false;
        }
        UAbility* ability = Cast<UAbility>(GetAbility(ability_num));
        if (ability)
        {
            return ability->CanBeLeveled(current_level);
        }
        return false;
    }

    virtual IAbilityInterface* GetAbility(int32 ability_num) const = 0;
    virtual int32 AbilityCount() const
    {
        return 10;
    }
};

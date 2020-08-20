// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityInterface.h"
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
    virtual IAbilityInterface* GetAbility(int32 ability_num) = 0;
    virtual int32 AbilityCount()
    {
        return 10;
    }
};

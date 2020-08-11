// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ManaInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UManaInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IManaInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual void AddToMana(float val)
    {
        auto new_mana = GetMana() + val;
        SetMana(new_mana);
    }

    virtual float GetMana() = 0;

    virtual void SetMana(float in_val) = 0;

    virtual void SetMaxMana(float in_val) = 0;

    virtual float GetMaxMana() const = 0;
};

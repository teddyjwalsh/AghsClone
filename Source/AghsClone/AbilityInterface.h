// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "AbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    AActor* GetAbilityOwner()
    {
        auto actor = Cast<AActor>(this);
        if (actor)
        {
            return actor->GetOwner();
        }
        else
        {
            auto component = Cast<UActorComponent>(this);
            if (component)
            {
                return component->GetOwner();
            }
        }
        return nullptr;
    }
	bool CostMana()
	{
		bool retval = false;
		auto mi = Cast<IManaInterface>(GetAbilityOwner());
        float ManaCost = GetManaCost();
		if (mi->GetMana() >= ManaCost)
		{
			mi->AddToMana(-ManaCost);
			retval = true;
		}
		else
		{
			retval = false;
		}
		return retval;
	}
	void OnActivationMeta() 
	{ 
		if (CostMana()) 
		{ 
			OnActivation();
		}
	}
	void OnUnitActivationMeta(FVector target)
	{
		if (CostMana())
		{
			OnUnitActivation(target);
		}
	}
	void OnGroundActivationMeta(FVector target)
	{
		if (CostMana())
		{
			OnGroundActivation(target);
		}
	}
    virtual float GetManaCost() const = 0;
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(FVector target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
	virtual void OnGroundActivation(FVector target) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
	}
};

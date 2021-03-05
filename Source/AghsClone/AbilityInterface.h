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
	bool OnActivationMeta()
	{
		bool retval = false;
		if (GetCurrentCooldown() == 0 || GetChargeCount() > 0)
		{
			if (GetChargeCount() > 0 || !IsCharges())
			{
				if (CostMana())
				{
					UseCharge();
					OnActivation();
					SetCurrentCooldown(GetCooldown());
					retval = true;
				}
			}
		}
		return retval;
	}
	bool OnUnitActivationMeta(AActor* target)
	{
		bool retval = false;
		if (GetCurrentCooldown() == 0 || GetChargeCount() > 0)
		{
			if (GetChargeCount() || !IsCharges())
			{
				if (CostMana() && GetCurrentCooldown() == 0)
				{
					UseCharge();
					OnUnitActivation(target);
					SetCurrentCooldown(GetCooldown());
					retval = true;
				}
			}
		}
		return retval;
	}
	bool OnGroundActivationMeta(const FVector& target)
	{
		bool retval = false;
		if (GetCurrentCooldown() == 0 || GetChargeCount() > 0)
		{
			if (GetChargeCount() || !IsCharges())
			{
				if (CostMana())
				{
					UseCharge();
					OnGroundActivation(target);
					SetCurrentCooldown(GetCooldown());
					retval = true;
				}
			}
		}
		return retval;
	}
	virtual float GetManaCost() const = 0;
	virtual float GetDamage() const
	{
		return 0;
	}
	virtual float GetCurrentCooldown() const
	{
		return 0;
	}
	virtual float GetCooldown() const
	{
		return 0;
	}
	virtual void SetCurrentCooldown(float in_val)
	{
	}
    virtual float GetCastRange() const
    {
        return 0;
    }
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(AActor* target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
    virtual void TickChannel(float DeltaTime) {}
    virtual bool IsChanneling() const { return false; }
    virtual void EndChannel() {}
    virtual float GetChannelTime() const { return 0; }
    virtual float GetMaxChannelTime() const { return 0; }
	virtual bool IsDoneChanneling() const { return true; }
	virtual void OnGroundActivation(const FVector& target)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
	}
    virtual bool IsToggleable() const
    {
        return false;
    }
    virtual bool IsToggled() const
    {
        return false;
    }
    virtual bool IsPassive() const
    {
        return false;
    }
    virtual bool IsUnitTargeted() const
    {
        return false;
    }
    virtual bool IsGroundTargeted() const
    {
        return false;
    }

	virtual bool IsCharges() const
	{
		return 0;
	}

	virtual bool UseCharge()
	{
		return true;
	}

	virtual int GetChargeCount() const
	{
		return -1;
	}

	virtual int GetMaxCharges() const
	{
		return -1;
	}

    virtual void Toggle() 
    {
    }

	virtual float GetCastPoint() const
	{
		return 0;
	}

	virtual float GetCastBackswing() const
	{
		return 0;
	}
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

enum DamageType
{
    PureDamage,
    PhysicalDamage,
    MagicDamage
};

struct DamageInstance
{
    float value;
    DamageType damage_type;
    bool is_attack;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IHealthInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual void AddToHealth(float val)
    {
        auto new_health = GetHealth() + val;
        SetHealth(new_health);
    }

    virtual float GetHealth() const = 0;

    virtual float GetDelayedHealth() const = 0;

    virtual void SetHealth(float in_val) = 0;

    virtual void SetMaxHealth(float in_val) = 0;

    virtual float GetArmor() const = 0;

    virtual float GetMagicResist() const = 0;

    virtual float GetMaxHealth() const = 0;

    virtual void ApplyDamage(float raw_value, DamageType damage_type)
    {
        switch (damage_type)
        {
        case PureDamage:
        {
            AddToHealth(-raw_value);
            break;
        }
        case PhysicalDamage:
        {
            float value = -raw_value*(1 - ((0.06 * GetArmor()) / (1 + 0.06 * abs(GetArmor()))));
            AddToHealth(value);
            break;
        }
        case MagicDamage:
        {
            float value = -(1 - GetMagicResist()) * raw_value;
            AddToHealth(value);
            break;
        }
        default:
            break;
        }
    }
};

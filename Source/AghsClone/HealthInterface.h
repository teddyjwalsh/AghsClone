// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BountyComponent.h"
#include "WalletComponent.h"
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
    virtual float AddToHealth(float val)
    {
        auto new_health = GetHealth() + val;
        SetHealth(new_health);
        return new_health;
    }

    virtual float GetHealth() const = 0;

    virtual float GetHealthRegen() const = 0;

    virtual void ApplyHealthRegen(float dt)
    {
        AddToHealth(GetHealthRegen() * dt);
    }

    virtual float GetDelayedHealth() const = 0;

    virtual void SetHealth(float in_val) = 0;

    virtual void SetMaxHealth(float in_val) = 0;

    virtual float GetArmor() const = 0;

    virtual float GetMagicResist() const = 0;

    virtual float GetMaxHealth() const = 0;

    //virtual void ApplyDamage(float raw_value, DamageType damage_type)
    virtual float ApplyDamage(DamageInstance& damage, AActor* damager)
    {
        float bounty = 0;
        float new_health;
        switch (damage.damage_type)
        {
        case PureDamage:
        {
            new_health = AddToHealth(-damage.value);
            break;
        }
        case PhysicalDamage:
        {
            float value = -damage.value*(1 - ((0.06 * GetArmor()) / (1 + 0.06 * abs(GetArmor()))));
            new_health = AddToHealth(value);
            break;
        }
        case MagicDamage:
        {
            float value = -(1 - GetMagicResist()) * damage.value;
            new_health = AddToHealth(value);
            break;
        }
        default:
            break;
        }
        if (new_health <= 0.1)
        {
            AActor* bounty_actor = Cast<AActor>(this);
            if (bounty_actor)
            {
                auto bounty_component = Cast<UBountyComponent>(bounty_actor->GetComponentByClass(UBountyComponent::StaticClass()));
                if (bounty_component)
                {
                    bounty = bounty_component->GetValue();
                    auto wallet_component = Cast<UWalletComponent>(damager->GetComponentByClass(UWalletComponent::StaticClass()));
                    if (wallet_component)
                    {
                        wallet_component->Deposit(bounty);
                    }
                }
            }
        }
        return bounty;  
    }
};

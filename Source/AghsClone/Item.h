// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "HealthInterface.h"
#include "AbilityInterface.h"
#include "StatInterface.h"

#include "Item.generated.h"

UCLASS()
class AGHSCLONE_API AItem : public AActor,
    public IAbilityInterface,
    public IStatInterface
{
	GENERATED_BODY()

	static TMap<FString, UTexture2D*> materials;
	static TMap<FString, int32> materials2;
	static bool materials_loaded;

public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(AItem, Health);
        DOREPLIFETIME(AItem, HealthRegen);
		DOREPLIFETIME(AItem, AttackSpeed);
        DOREPLIFETIME(AItem, Mana);
        DOREPLIFETIME(AItem, ManaRegen);
		DOREPLIFETIME(AItem, AttackDamage);
		DOREPLIFETIME(AItem, Armor);
        DOREPLIFETIME(AItem, MyMat);
        DOREPLIFETIME(AItem, Cooldown);
        DOREPLIFETIME(AItem, CurrentCooldown);
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual float GetStat(StatType stat_type) const override
    {
        if (stats[int32(stat_type)])
        {
            return *stats[int32(stat_type)];
        }
        return 0;
    }

    virtual bool SetStat(StatType stat_type, float in_stat) override
    {
        if (stats[int32(stat_type)])
        {
            *stats[int32(stat_type)] = in_stat;
            return true;
        }
        return false;
    }

    bool AddStat(StatType stat_type, float* in_stat = nullptr)
    {
        if (!in_stat)
        {
            stats[stat_type] = new float(0);
        }
        else
        {
            stats[stat_type] = in_stat;
        }
        return true;
    }

	void SetMaterial(FString to_load)
	{
        if (!materials.Find(to_load))
        {
            //ConstructorHelpers::FObjectFinder<UTexture2D> ScreenMat(*FString::Printf(TEXT("Texture2D'/Game/Textures/%s.%s'"), *to_load, *to_load));
            auto item_tex = Cast<UTexture2D>(
                StaticLoadObject(UTexture2D::StaticClass(), 
                    NULL, 
                    *FString::Printf(TEXT("/Game/Textures/%s.%s"), 
                        *to_load, 
                        *to_load)));
            materials.Add(to_load, item_tex);
        }
		MyMat = materials[to_load];
	}

    virtual float GetCooldown() const override
    {
        return Cooldown;
    }
    virtual float GetCurrentCooldown() const override
    {
        return CurrentCooldown;
    }
    virtual void SetCurrentCooldown(float in_val) override
    {
        CurrentCooldown = in_val;
    }

	UTexture2D* GetMaterial()
	{
		return MyMat;
	}

    virtual void OnDamage(DamageInstance& damage)
    {
    }

    virtual void OnAttacked(DamageInstance& damage)
    {
    }

    virtual void OnHit(DamageInstance& damage, AActor* unit_attacked) override
    {
    }

    virtual void OnActivation() override
    {
    }

    virtual void OnGroundActivation(const FVector& ground_target) override
    {
    }

    virtual void OnToggle()
    {
    }

    virtual void OnUnitActivation(AActor* unit) override
    {
    }

    virtual float GetCastRange() const override
    {
        return CastRange;
    }
    virtual float GetManaCost() const override
    {
        return ManaCost;
    }

    virtual bool IsToggleable() const override
    {
        return bToggleable;
    }
    virtual bool IsToggled() const override
    {
        return bToggled;
    }
    virtual bool IsPassive() const override
    {
        return bPassive;
    }
    virtual bool IsUnitTargeted() const override
    {
        return bUnitTargeted;
    }
    virtual bool IsGroundTargeted() const override
    {
        return bGroundTargeted;
    }
    virtual void Toggle() override
    {
        bToggled = !bToggled;
    }

	class USphereComponent* bounds;


	UPROPERTY( Replicated )
	UTexture2D* MyMat;

    UPROPERTY(Replicated)
    float Cooldown;
    UPROPERTY(Replicated)
    float CurrentCooldown;

	UPROPERTY(Replicated)
    bool bActive;
	UPROPERTY(Replicated)
    bool bToggleable;
	UPROPERTY(Replicated)
    bool bUnitTargeted;
	UPROPERTY(Replicated)
    bool bGroundTargeted;
	UPROPERTY(Replicated)
    bool bOnHit;
	bool bToggled = false;
	bool bPassive = false;
    
    TArray<float*> stats;

	UPROPERTY(Replicated)
	float Movespeed;
	UPROPERTY(Replicated)
	float CastRange;
	UPROPERTY(Replicated)
	float ManaCost;
	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float HealthRegen;
	UPROPERTY(Replicated)
	float ManaRegen;
	UPROPERTY(Replicated)
	float AttackDamage;
	UPROPERTY(Replicated)
	float AttackSpeed;
	UPROPERTY(Replicated)
	float Armor;
    UPROPERTY(Replicated)
    float MagicResist;
	UPROPERTY(Replicated)
	float Strength;
	UPROPERTY(Replicated)
	float Agility;
	UPROPERTY(Replicated)
	float Intelligence;
};

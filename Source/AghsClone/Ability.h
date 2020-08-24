// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "AbilityInterface.h"
#include "Ability.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UAbility : public UActorComponent,
    public IAbilityInterface
{
	GENERATED_BODY()

    static TMap<FString, UTexture2D*> materials;

    UPROPERTY(Replicated)
    UTexture2D* MyMat;

public:	
	// Sets default values for this component's properties
	UAbility();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	bool bUnitTargeted = false;
	bool bGroundTargeted = false;
	bool bToggleable = false;
	bool bToggled = false;
	bool bPassive = false;
    bool bOnHit = false;
    bool bIsUltimate = false;
	float DefaultCooldown;
    UPROPERTY( Replicated )
	float ManaCost;
	float CastRange;
    UPROPERTY(Replicated)
    float Cooldown;
    UPROPERTY(Replicated)
    float CurrentCooldown;
	class UDecalComponent* TargetingDecal;
    UPROPERTY()
    int32 max_level;
    UPROPERTY(Replicated)
    int32 current_level;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UAbility, ManaCost);
        DOREPLIFETIME(UAbility, Cooldown);
        DOREPLIFETIME(UAbility, MyMat);
        DOREPLIFETIME(UAbility, CurrentCooldown);
        DOREPLIFETIME(UAbility, current_level);
    }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual float GetMaxLevel() const
    {
        return max_level;
    }
    virtual float GetLevel() const
    {
        return current_level;
    }
    virtual bool LevelUp()
    {
        if (current_level < max_level)
        {
            current_level += 1;
            return true;
        }
        return false;
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
    virtual float GetCastRange() const
    {
        return CastRange;
    }
	virtual float GetManaCost() const override 
    {
        return ManaCost;
    }
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(AActor* target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
	virtual void OnGroundActivation(const FVector& target) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
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
    virtual bool IsUltimate() const
    {
        return bIsUltimate;
    }
    virtual bool CanBeLeveled(int32 in_level)
    {
        if (bIsUltimate)
        {
            if (in_level >= 18)
            {
                return true;
            }
            else if (in_level >= 12 && GetLevel() <= 1)
            {
                return true;
            }
            else if (in_level >= 6 && GetLevel() <= 0)
            {
                return true;
            }
        }
        else
        {
            if (in_level >= 7)
            {
                return true;
            }
            else if (in_level >= 5 && GetLevel() < 3)
            {
                return true;
            }
            else if (in_level >= 3 && GetLevel() < 2)
            {
                return true;
            }
            else if (in_level >= 1 && GetLevel() < 1)
            {
                return true;
            }
        }
        return false;
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
            if (item_tex)
            {
                materials.Add(to_load, item_tex);
                MyMat = materials[to_load];
            }
        }
        else
        {
            MyMat = materials[to_load];
        }
        
    }

    UTexture2D* GetMaterial()
    {
        return MyMat;
    }
};

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
	float DefaultCooldown;
	float ManaCost;
	float CastRange;
    float Cooldown;
    float CurrentCooldown;
	class UDecalComponent* TargetingDecal;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UAbility, ManaCost);
        DOREPLIFETIME(UAbility, Cooldown);
        DOREPLIFETIME(UAbility, MyMat);
        DOREPLIFETIME(UAbility, CurrentCooldown);
    }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
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

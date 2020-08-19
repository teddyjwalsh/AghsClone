// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "AbilityInterface.h"
#include "Ability.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UAbility : public UActorComponent,
    public IAbilityInterface
{
	GENERATED_BODY()

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
	class UDecalComponent* TargetingDecal;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual float GetManaCost() const override 
    {
        return ManaCost;
    }
	virtual void OnActivation() override { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(FVector target) override { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) override { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
	virtual void OnGroundActivation(FVector target) override
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
	}
};

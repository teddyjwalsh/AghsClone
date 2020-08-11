// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ManaInterface.h"
#include "Ability.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UAbility : public UActorComponent
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
	float DefaultCooldown;
	float ManaCost;
	float CastRange;
	class UDecalComponent* TargetingDecal;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool CostMana()
	{
		bool retval = false;
		auto mi = Cast<IManaInterface>(GetOwner());
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
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(FVector target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnGroundActivation(FVector target) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
	}
};

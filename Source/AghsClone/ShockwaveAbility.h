// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Shockwave.h"
#include "ShockwaveAbility.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UShockwaveAbility : public UAbility
{
	GENERATED_BODY()

	std::vector<AShockwave*> instances;

	// Sets default values for this component's properties
	UShockwaveAbility()
	{
		// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
		// off to improve performance if you don't need them.
		PrimaryComponentTick.bCanEverTick = false;
		ManaCost = 40;
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		PrimaryComponentTick.bCanEverTick = false;
	}

	virtual void OnActivation() override
	{
		//auto new_instance = NewObject<UBallDrop>(this);
		auto new_instance = GetWorld()->SpawnActor<AShockwave>();
		new_instance->SetOwner(GetOwner());
		new_instance->SetActorLocation(GetOwner()->GetActorLocation() + FVector(500,0,0));
		//new_instance->SetPos(target + FVector(0, 0, 200));
		instances.push_back(new_instance);
	}
};

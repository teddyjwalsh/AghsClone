// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <vector>

#include "CoreMinimal.h"
#include "Ability.h"
#include "BallDrop.h"
#include "Components/SphereComponent.h"
#include "BallDropAbility.generated.h"
/*
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGHSCLONE_API UBallDrop : public UActorComponent
{
	GENERATED_BODY()

	FVector start_pos;
	USphereComponent* sphere;

public:
	// Sets default values for this component's properties
	UBallDrop()
	{
		PrimaryComponentTick.bCanEverTick = true;
		sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
		
		//sphere->SetupAttachment(RootComponent);
	}
	
	void SetPos(FVector in_pos)
	{
		pos = in_pos;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override
	{
		Super::BeginPlay();
	}

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		pos.Z -= 2;
	}
};
*/

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UBallDropAbility : public UAbility
{
	GENERATED_BODY()

	UBallDropAbility();

	std::vector<ABallDrop*> instances;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnGroundActivation(FVector target) override
	{
		//auto new_instance = NewObject<UBallDrop>(this);
		auto new_instance = GetWorld()->SpawnActor<ABallDrop>();
		new_instance->SetActorLocation(target + FVector(0, 0, 500));
		//new_instance->SetPos(target + FVector(0, 0, 200));
		instances.push_back(new_instance);
	}
};


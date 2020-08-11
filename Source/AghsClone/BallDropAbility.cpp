// Fill out your copyright notice in the Description page of Project Settings.


#include "BallDropAbility.h"

// Sets default values for this component's properties
UBallDropAbility::UBallDropAbility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bGroundTargeted = true;
	CastRange = 500;
	ManaCost = 20;
}


// Called when the game starts
void UBallDropAbility::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UBallDropAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
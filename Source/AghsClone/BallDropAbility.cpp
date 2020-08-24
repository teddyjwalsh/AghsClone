// Fill out your copyright notice in the Description page of Project Settings.


#include "BallDropAbility.h"

// Sets default values for this component's properties
UBallDropAbility::UBallDropAbility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bGroundTargeted = true;
	CastRange = 500;
	ManaCost = 20;
	Cooldown = 10;
	max_level = 4;
	current_level = 0;
}


// Called when the game starts
void UBallDropAbility::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

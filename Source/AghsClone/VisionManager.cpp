// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "VisionManager.h"

// Sets default values
AVisionManager::AVisionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVisionManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (TActorIterator<AAghsCloneCharacter> act_it(GetWorld()); act_it; ++act_it)
	{
		/*
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UVisionInterface::StaticClass(), )
		vision_bounds->SetWorldLocation(act_it->GetActorLocation());
		vision_bounds->SetSphereRadius(act_it->GetVisionRadius());
		TSet<AActor*> near_chars;
		vision_bounds->GetOverlappingActors(near_chars, AAghsCloneCharacter::StaticClass());
		*/

	}
}


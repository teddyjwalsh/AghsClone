// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "VisionInterface.h"
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

	TArray<AActor*> vision_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UVisionInterface::StaticClass(), vision_actors);

	for (auto& act : vision_actors)
	{
		IVisionInterface* vision_interface = Cast<IVisionInterface>(act);
		vision_bounds->SetWorldLocation(act->GetActorLocation());
		vision_bounds->SetSphereRadius(vision_interface->GetVisionRadius());
		TSet<AActor*> near_chars;
		vision_bounds->GetOverlappingActors(near_chars, AAghsCloneCharacter::StaticClass());

	}
}


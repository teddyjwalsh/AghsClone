// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>
#include <set>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "AghsCloneCharacter.h"
#include "VisionManager.generated.h"

UCLASS()
class AGHSCLONE_API AVisionManager : public AActor
{
	GENERATED_BODY()

	std::map<int32, std::set<AActor*>> team_vision_sets;
	USphereComponent* vision_bounds;

public:	
	// Sets default values for this actor's properties
	AVisionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

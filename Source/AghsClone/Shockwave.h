// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Shockwave.generated.h"

UCLASS()
class AGHSCLONE_API AShockwave : public AActor
{
	GENERATED_BODY()
	
	UStaticMeshComponent* sphere;
	USphereComponent* bounds;
	
	float scale = 0.1;
	std::set<int32> hit;

public:	
	// Sets default values for this actor's properties
	AShockwave();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTouch(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);


};

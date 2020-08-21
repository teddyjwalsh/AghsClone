// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldActorInterface.h"
#include "Components/SphereComponent.h"
#include "StatusManager.h"
#include "Shockwave.generated.h"

UCLASS(Blueprintable)
class UShockwaveSlow : public UStatusEffect
{
	GENERATED_BODY()

	float slow;

public:
	UShockwaveSlow()
	{
		max_duration = 3;
		slow = -100;
		AddStat(StatMovespeed, &slow);
	}
};

UCLASS()
class AGHSCLONE_API AShockwave : public AActor,
	public IFieldActorInterface
{
	GENERATED_BODY()
	
	UStaticMeshComponent* sphere;
	USphereComponent* bounds;
	
	float scale = 0.1;
	//std::set<int32> hit;
	TSet<AActor*> hit;

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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackProjectile.generated.h"

UCLASS()
class AGHSCLONE_API AAttackProjectile : public AActor
{
	GENERATED_BODY()

	class UStaticMeshComponent* model;

	AActor* target;
	float speed;
	
public:	
	// Sets default values for this actor's properties
	AAttackProjectile();

	void SetTarget(AActor* in_target)
	{
		target = in_target;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

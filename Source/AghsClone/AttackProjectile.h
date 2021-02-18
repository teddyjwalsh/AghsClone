// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldActorInterface.h"
#include "AttackProjectile.generated.h"

UCLASS()
class AGHSCLONE_API AAttackProjectile : public AActor,
	public IFieldActorInterface
{
	GENERATED_BODY()

	

	AActor* target;
	AActor* owner;
	float speed;
	
public:
	class UStaticMeshComponent* model;
	// Sets default values for this actor's properties
	AAttackProjectile();

	void SetTarget(AActor* in_target)
	{
		target = in_target;
	}

	AActor* GetTarget()
	{
		return target;
	}

	void SetShooter(AActor* in_owner)
	{
		owner = in_owner;
	}

	void SetSpeed(float in_speed)
	{
		speed = in_speed;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

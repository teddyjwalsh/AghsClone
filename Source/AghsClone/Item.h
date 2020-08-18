// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Item.generated.h"

UCLASS()
class AGHSCLONE_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AItem, Health);
		DOREPLIFETIME(AItem, AttackSpeed);
		DOREPLIFETIME(AItem, Mana);
		DOREPLIFETIME(AItem, AttackDamage);
		DOREPLIFETIME(AItem, Armor);
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	class USphereComponent* bounds;

	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float HealthRegen;
	UPROPERTY(Replicated)
	float ManaRegen;
	UPROPERTY(Replicated)
	float AttackDamage;
	UPROPERTY(Replicated)
	float AttackSpeed;
	UPROPERTY(Replicated)
	float Armor;
	UPROPERTY(Replicated)
	float Strength;
	UPROPERTY(Replicated)
	float Agility;
	UPROPERTY(Replicated)
	float Intelligence;
};

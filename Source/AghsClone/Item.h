// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

#include "HealthInterface.h"
#include "AbilityInterface.h"

#include "Item.generated.h"

UCLASS()
class AGHSCLONE_API AItem : public AActor,
    public IAbilityInterface
{
	GENERATED_BODY()

	static TMap<FString, UTexture2D*> materials;
	static TMap<FString, int32> materials2;
	static bool materials_loaded;

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
		DOREPLIFETIME(AItem, MyMat);
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMaterial(FString mat_name)
	{
		MyMat = materials[mat_name];
	}

	UTexture2D* GetMaterial()
	{
		return MyMat;
	}

    virtual void OnDamage(DamageInstance& damage)
    {
    }

    virtual void OnAttacked(DamageInstance& damage)
    {
    }

    virtual void OnHit(DamageInstance& damage, AActor* unit_attacked) override
    {
    }

    virtual void OnActivation() override
    {
    }

    virtual void OnGroundActivation(const FVector& ground_target) override
    {
    }

    virtual void OnToggle()
    {
    }

    virtual void OnUnitActivation(AActor* unit) override
    {
    }

    virtual float GetManaCost() const override
    {
        return ManaCost;
    }

	class USphereComponent* bounds;


	UPROPERTY( Replicated )
	UTexture2D* MyMat;


	UPROPERTY(Replicated)
    bool bActive;
	UPROPERTY(Replicated)
    bool bToggleable;
	UPROPERTY(Replicated)
    bool bUnitTargeted;
	UPROPERTY(Replicated)
    bool bGroundTargeted;
	UPROPERTY(Replicated)
    bool bOnHit;
    
	UPROPERTY(Replicated)
	float ManaCost;
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

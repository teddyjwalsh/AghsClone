// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "WalletComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UWalletComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	float money = 2000;

public:	
	// Sets default values for this component's properties
	UWalletComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool Debit(float amount)
	{
		if (money >= amount)
		{
			money -= amount;
			return true;
		}
		return false;
	}
	
	virtual bool Deposit(float amount)
	{
		money += amount;
		return true;
	}

	virtual float Balance() const
	{
		return money;
	}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UWalletComponent, money);
	}
};

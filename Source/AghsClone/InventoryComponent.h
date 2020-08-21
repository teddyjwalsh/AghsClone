// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "InventoryComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TArray<AItem*> items;

	const int32 slot_count = 6;

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

	bool InsertItem(AItem* in_item, int32 slot_number)
	{
		if (slot_number < slot_count)
		{
			items[slot_number] = in_item;
			in_item->SetOwner(GetOwner());
			return true;
		}
		return false;
	}

	bool InsertItem(AItem* in_item)
	{
		for (auto& it : items)
		{
			if (it == nullptr)
			{
				it = in_item;
				in_item->SetOwner(GetOwner());
				return true;
			}
		}
		return false;
	}

	AItem* RemoveItem(int32 slot_number)
	{
		auto temp_item = items[slot_number];
		items[slot_number] = nullptr;
		return temp_item;
	}

	void SwapSlots(int32 slot_number1, int32 slot_number2)
	{
		auto temp_item = items[slot_number1];
		items[slot_number1] = items[slot_number2];
		items[slot_number2] = temp_item;
	}

	float GetHealth() const
	{
		float health_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				health_sum += it->Health;
			}
		}
		return health_sum;
	}

	float GetHealthRegen() const
	{
		float health_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				health_sum += it->HealthRegen;
			}
		}
		return health_sum;
	}

	float GetMana() const
	{
		float mana_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				mana_sum += it->Mana;
			}
		}
		return mana_sum;
	}

	float GetManaRegen() const
	{
		float mana_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				mana_sum += it->ManaRegen;
			}
		}
		return mana_sum;
	}

	float GetAttackDamage() const
	{
		float mana_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				mana_sum += it->AttackDamage;
			}
		}
		return mana_sum;
	}

	float GetAttackSpeed() const
	{
		float mana_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				mana_sum += it->AttackSpeed;
			}
		}
		return mana_sum;
	}

	float GetMovespeed() const
	{
		float mana_sum = 0;
		for (auto& it : items)
		{
			if (it != nullptr)
			{
				mana_sum += it->Movespeed;
			}
		}
		return mana_sum;
	}

    void OnHit(DamageInstance& damage, AActor* unit)
    {
		for (auto& it : items)
		{
			if (it)
			{
				if (it->bOnHit)
				{
					it->OnHit(damage, unit);
				}
			}
        }
    }
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UInventoryComponent, items);
	}

    IAbilityInterface* GetAbility(int32 inventory_index) const
    {
        return Cast<IAbilityInterface>(items[inventory_index]);
    }

	TArray<AItem*>& GetItems()
	{
		return items;
	}
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};

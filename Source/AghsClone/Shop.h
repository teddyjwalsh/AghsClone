// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "Shop.generated.h"

typedef int32 ItemId;

UCLASS()
class AGHSCLONE_API AShop : public AActor
{
	GENERATED_BODY()

	static TArray<AShop*> shops;
	TArray<ItemId> items;
	std::map<ItemId, int32> stock;
	float radius;

	AShop();

public:	
	// Sets default values for this actor's properties


	AItem* BuyItem(ItemId item)
	{
		int32 item_index;
		if (items.Find(item, item_index))
		{
			if (stock.find(item) == stock.end())
			{
				auto new_item = GetWorld()->SpawnActor<AItem>();
				return new_item;
			}
			else
			{
				if (stock[item] > 0)
				{
					auto new_item = GetWorld()->SpawnActor<AItem>();
					stock[item] -= 1;
					return new_item;
				}
			}
		}
		return nullptr;
	}

	bool InStock(ItemId item, int32& stock_count)
	{
		int32 item_index;
		if (items.Find(item, item_index))
		{
			if (stock.find(item) == stock.end())
			{
				stock_count = -1;
				return true;
			}
			else
			{
				if (stock[item] > 0)
				{
					stock_count = stock[item];
					return true;
				}
			}
		}
		stock_count = 0;
		return false;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static AShop* CreateShop(UWorld* in_world, const FVector& location, const TArray<ItemId>& in_items, float in_radius = 800)
	{
		AShop* new_shop = in_world->SpawnActor<AShop>(location, FRotator());// (in_items, in_radius);
		new_shop->items = in_items;
		new_shop->radius = in_radius;
		shops.Add(new_shop);
		return shops.Last();
	}

	static AShop* GetClosestShop(AActor* in_actor, ItemId item_id = -1)
	{
		for (auto& s : shops)
		{
			if ((in_actor->GetActorLocation() - s->GetActorLocation()).Size() < s->radius)
			{
				int32 item_stock;
				if (s->InStock(item_id, item_stock))
				{
					return s;
				}
			}
		}
		return nullptr;
	}


};

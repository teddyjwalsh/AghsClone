// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "Shop.generated.h"

typedef int32 ItemId;

struct ShopItemSlot
{
	float price;
	int32 stock = -1;
	AItem* proto;
	UClass* item_class = AItem::StaticClass();
};

UCLASS()
class AGHSCLONE_API AShop : public AActor
{
	GENERATED_BODY()

	static TArray<AShop*> shops;
	static AShop* default_shop;
	TMap<ItemId, ShopItemSlot> items;
	float radius;

	AShop();

public:	
	// Sets default values for this actor's properties


	AItem* BuyItem(ItemId item)
	{
		if (items.Contains(item))
		{
			if (items[item].stock == -1)
			{
				FActorSpawnParameters act_par;
				act_par.Template = items[item].proto;
				auto new_item = GetWorld()->SpawnActor<AItem>(items[item].item_class, act_par);
				return new_item;
			}
			else
			{
				if (items[item].stock > 0)
				{
					FActorSpawnParameters act_par;
					act_par.Template = items[item].proto;
					auto new_item = GetWorld()->SpawnActor<AItem>(items[item].item_class, act_par);
					items[item].stock -= 1;
					return new_item;
				}
			}
		}
		return nullptr;
	}

	bool InStock(ItemId item, int32& stock_count)
	{
		if (items.Contains(item))
		{
			if (items[item].stock == -1)
			{
				stock_count = -1;
				return true;
			}
			else
			{
				if (items[item].stock > 0)
				{
					stock_count = items[item].stock;
					return true;
				}
			}
		}
		stock_count = 0;
		return false;
	}

	float GetPrice(ItemId in_item) const
	{
		return items[in_item].price;
	}

	bool AddItem(AItem* in_item, ItemId id, float price, int32 in_stock = -1)
	{
		ShopItemSlot new_slot;
		new_slot.price = price;
		new_slot.stock = in_stock;
		new_slot.proto = in_item;
		new_slot.item_class = in_item->GetClass();
		items.Add(id, new_slot);
		return true;
	}

	const TMap<ItemId, ShopItemSlot>& GetItems()
	{
		return items;
	}

	bool CanBuy(AActor* in_actor) const
	{
		return (in_actor->GetActorLocation() - GetActorLocation()).Size() < radius;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static AShop* CreateShop(UWorld* in_world, const FVector& location, float in_radius = 800)
	{
		AShop* new_shop = in_world->SpawnActor<AShop>(location, FRotator());
		if (!IsValid(default_shop))
		{
			default_shop = new_shop;
		}
		new_shop->radius = in_radius;
		shops.Add(new_shop);
		return shops.Last();
	}

	static void DestroyShops()
	{
		shops.Empty();
	}

	static AShop* GetClosestShop(AActor* in_actor, ItemId item_id = -1)
	{
		for (auto& s : shops)
		{
			if ((in_actor->GetActorLocation() - s->GetActorLocation()).Size() < s->radius)
			{
				if (item_id == -1)
				{
					return s;
				}
				int32 item_stock;
				if (s->InStock(item_id, item_stock))
				{
					return s;
				}
			}
		}
		return default_shop;
	}


};

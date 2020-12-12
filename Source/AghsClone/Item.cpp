// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
//#include "Factories/MaterialFactoryNew.h"

std::vector<FString> mats_to_load = {};// "kotlguyp", "moon_shard", "brown_boots", "serpentblade", "blink_dagger" };

TMap<FString, UTexture2D*> AItem::materials;
TMap<FString, int32> AItem::materials2;
bool AItem::materials_loaded = false;

void CreateMaterial()
{
    UMaterial* UnrealMaterial = NewObject<UMaterial>();
	UMaterialExpressionTextureBase* tex = NewObject<UMaterialExpressionTextureBase>(UnrealMaterial);
}

// Sets default values
AItem::AItem()
{
	for (int i = START_STAT_TYPE; i != END_STAT_TYPE; ++i)
	{
		stats.Add(nullptr);
	}
	for (int i = START_STAT_TYPE; i != END_STAT_TYPE; ++i)
	{
		mult_stats.Add(nullptr);
	}
	AddStat(StatMaxHealth, &Health);
	AddStat(StatMaxMana, &Mana);
	AddStat(StatArmor, &Armor);
	AddStat(StatAttackSpeed, &AttackSpeed);
	AddStat(StatMovespeed, &Movespeed);
	AddStat(StatAttackDamage, &AttackDamage);
	AddStat(StatMagicResist, &MagicResist);
	AddStat(StatHealthRegen, &HealthRegen);
	AddStat(StatManaRegen, &ManaRegen);
	if (!materials_loaded)
	{
		for (auto& to_load : mats_to_load)
		{
			ConstructorHelpers::FObjectFinder<UTexture2D> ScreenMat(*FString::Printf(TEXT("Texture2D'/Game/Textures/%s.%s'"), *to_load, *to_load));
			materials.Add(to_load, ScreenMat.Object);
		}
		materials_loaded = true;
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bReplicates = true;
	bounds = CreateDefaultSubobject<USphereComponent>(TEXT("bounds"));
	SetRootComponent(bounds);
	bounds->SetIsReplicated(true);

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CurrentCooldown > 0)
	{
		CurrentCooldown = std::max(0.0f, CurrentCooldown - DeltaTime);
	}
}


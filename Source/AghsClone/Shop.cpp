// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"

TArray<AShop*> AShop::shops = TArray<AShop*>();
AShop* AShop::default_shop = nullptr;

// Sets default values
AShop::AShop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	radius_component = CreateDefaultSubobject<USphereComponent>(TEXT("radius"));
	SetRootComponent(radius_component);
}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
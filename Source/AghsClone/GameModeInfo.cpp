// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeInfo.h"
#include "CrystalMaidenHero.h"
#include "SniperHero.h"

// Sets default values
AGameModeInfo::AGameModeInfo()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	FCharacterSpec temp_char_spec;// NewObject<FCharacterSpec>(GetWorld());
	temp_char_spec.SetTexture("crystal_maiden");
	temp_char_spec.CharClass = ACrystalMaidenHero::StaticClass();
	character_list.Add(temp_char_spec);

	temp_char_spec.SetTexture("sniper");
	temp_char_spec.CharClass = ASniperHero::StaticClass();
	character_list.Add(temp_char_spec);
}

// Called when the game starts or when spawned
void AGameModeInfo::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameModeInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


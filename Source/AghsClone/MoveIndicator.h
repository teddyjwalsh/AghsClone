// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MoveIndicator.generated.h"

UCLASS()
class AGHSCLONE_API AMoveIndicator : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* MoveIndicatorDecal;

	ConstructorHelpers::FObjectFinder<UMaterial>* DecalMaterialAsset;
	UMaterialInstanceDynamic* DynDecalMaterial;
	float anim_time;

public:	
	// Sets default values for this actor's properties
	AMoveIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveIndicator.h"
#include "Components/DecalComponent.h"

// Sets default values
AMoveIndicator::AMoveIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MoveIndicatorDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MoveIndicatorDecal"));
	MoveIndicatorDecal->SetupAttachment(RootComponent);
	DecalMaterialAsset = new ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Materials/MoveIndicatorDecal.MoveIndicatorDecal'"));
}

// Called when the game starts or when spawned
void AMoveIndicator::BeginPlay()
{
	Super::BeginPlay();
	DynDecalMaterial = UMaterialInstanceDynamic::Create(DecalMaterialAsset->Object, this);
	anim_time = 0;
	if (DecalMaterialAsset->Succeeded())
	{
		MoveIndicatorDecal->SetDecalMaterial(DynDecalMaterial);
	}
	MoveIndicatorDecal->DecalSize = FVector(100.0f, 50.0f, 50.0f);
	MoveIndicatorDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	MoveIndicatorDecal->SetRelativeLocation(FVector(0, 0, 95));
	MoveIndicatorDecal->SetSortOrder(0);
}

// Called every frame
void AMoveIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	anim_time += DeltaTime;
	DynDecalMaterial->SetScalarParameterValue("AnimTime", anim_time);
}


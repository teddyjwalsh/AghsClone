// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability.h"
#include "Components/DecalComponent.h"

// Sets default values for this component's properties
UAbility::UAbility()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Create a decal in the world to show the cursor's location
	TargetingDecal = CreateDefaultSubobject<UDecalComponent>("TargetingDecal");
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		TargetingDecal->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	TargetingDecal->DecalSize = FVector(16.0f, 128.0f, 128.0f);
	TargetingDecal->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	// ...
}


// Called when the game starts
void UAbility::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAbility::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


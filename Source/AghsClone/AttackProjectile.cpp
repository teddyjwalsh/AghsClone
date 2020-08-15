// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackProjectile.h"

// Sets default values
AAttackProjectile::AAttackProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	model = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");
	model->SetStaticMesh(ProjectileMesh.Object);
}

// Called when the game starts or when spawned
void AAttackProjectile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAttackProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(target))
	{
		FVector move_vector = target->GetActorLocation() - GetActorLocation();
		move_vector.Normalize();
		SetActorLocation(GetActorLocation() + move_vector * speed * DeltaTime);
	}
}


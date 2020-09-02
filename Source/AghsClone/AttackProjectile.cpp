// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackProjectile.h"

#include "AghsCloneCharacter.h"

// Sets default values
AAttackProjectile::AAttackProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	SetRootComponent(model);
	model = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");
	model->SetStaticMesh(ProjectileMesh.Object);
	speed = 800;
	model->SetCollisionProfileName(TEXT("Projectile"));
	model->SetWorldScale3D(FVector(0.2));
	//SetActorEnableCollision(false);
	model->OnComponentBeginOverlap.AddDynamic(this, &AAttackProjectile::OnOverlap);
	model->SetIsReplicated(true);
	bReplicates = true;
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
	if (IsValid(target) && !target->IsPendingKill())
	{
		FVector move_vector = target->GetActorLocation() - GetActorLocation();
		move_vector.Normalize();
		SetActorLocation(GetActorLocation() + move_vector * speed * DeltaTime);
	}
	else
	{
		Destroy();
	}
}

void AAttackProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == target)
	{
		auto character = Cast<IHealthInterface>(target);
		auto owner_char = Cast<AAghsCloneCharacter>(owner);
		if (auto unit = Cast<AAghsCloneCharacter>(target))
		{
			owner_char->ApplyOnHit(unit);
		}
		Destroy();
	}
}


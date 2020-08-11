// Fill out your copyright notice in the Description page of Project Settings.


#include "BallDrop.h"
#include "AghsCloneCharacter.h"

// Sets default values
ABallDrop::ABallDrop()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ScreenMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sphere = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");
	sphere->SetStaticMesh(ScreenMesh.Object);
	SetRootComponent(sphere);
	bounds = CreateDefaultSubobject<USphereComponent>("Bounds");
	sphere->SetSimulatePhysics(true);
	SetActorEnableCollision(true);
	sphere->SetMobility(EComponentMobility::Movable);
	sphere->SetNotifyRigidBodyCollision(true);
	sphere->OnComponentHit.AddDynamic(this, &ABallDrop::OnTouch);
}

// Called when the game starts or when spawned
void ABallDrop::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABallDrop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//AddActorLocalOffset(FVector(0, 0, -5));
	if (GetActorLocation().Z < 0)
	{
		Destroy();
	}
}

void ABallDrop::OnTouch(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	auto health_comp = Cast<IHealthInterface>(OtherActor);
	if (health_comp != nullptr)
	{
		health_comp->ApplyDamage(20, MagicDamage);
		UE_LOG(LogTemp, Warning, TEXT("HIT CHARACTER: %f"), health_comp->GetHealth());
	}
}

/*
void ABallDrop::NotifyHit
(
	class UPrimitiveComponent* MyComp,
	AActor* Other,
	class UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	UE_LOG(LogTemp, Warning, TEXT("HIT Something"));
}
*/
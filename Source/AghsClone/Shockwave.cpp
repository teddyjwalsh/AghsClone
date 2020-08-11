// Fill out your copyright notice in the Description page of Project Settings.


#include "Shockwave.h"
#include "HealthInterface.h"

// Sets default values
AShockwave::AShockwave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ScreenMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> ScreenMat(TEXT("MaterialInstanceConstant'/Game/StarterContent/Materials/M_Glass.M_Glass'"));
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sphere = CreateDefaultSubobject<UStaticMeshComponent>("Sphere");
	bounds = CreateDefaultSubobject<USphereComponent>("Bounds");
	bounds->SetupAttachment(sphere);
	sphere->SetStaticMesh(ScreenMesh.Object);
	SetRootComponent(sphere);
	sphere->SetWorldScale3D(FVector(scale));
	sphere->SetSimulatePhysics(false);
	
	//sphere->OnComponentHit.AddDynamic(this, &AShockwave::OnTouch);
	sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bounds->SetGenerateOverlapEvents(true);
	sphere->SetCastShadow(false);
	sphere->SetMaterial(0, ScreenMat.Object);
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AShockwave::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShockwave::Tick(float DeltaTime)
{
	TSet<AActor*> set;
	bounds->GetOverlappingActors(set);
	if (set.Num() > 0)
	{
		for (auto& act : set)
		{
			int32 act_id = act->GetUniqueID();
			if (hit.find(act_id) == hit.end() && act != GetOwner())
			{
				if (bounds->GetScaledSphereRadius() - (act->GetActorLocation() - GetActorLocation()).Size() < 20)
				{
					auto health_comp = Cast<IHealthInterface>(act);
					if (health_comp)
					{
						health_comp->ApplyDamage(20, MagicDamage);
						hit.insert(act->GetUniqueID());
						UE_LOG(LogTemp, Warning, TEXT("SHOCK HIT CHARACTER: %f"), health_comp->GetHealth());
					}
				}
			}
		}
	}
	Super::Tick(DeltaTime);
	scale += 0.5;
	sphere->SetWorldScale3D(FVector(scale));
	if (scale > 40)
	{
		Destroy();
	}
}

void AShockwave::OnTouch(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//if (OtherActor->IsA(AAghsCloneCharacter::StaticClass()))
	auto health_comp = Cast<IHealthInterface>(OtherActor);
	if (health_comp)
	{
		health_comp->AddToHealth(-20);
		//UE_LOG(LogTemp, Warning, TEXT("SHOCK HIT CHARACTER: %f"), health_comp->GetHealth());
	}

	//UE_LOG(LogTemp, Warning, TEXT("HIT Something"));

}
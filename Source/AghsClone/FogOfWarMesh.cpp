// Fill out your copyright notice in the Description page of Project Settings.

#include "FogOfWarMesh.h"

#include "Components/SphereComponent.h"
#include "FieldActorInterface.h"
#include "AghsCloneCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFogOfWarMesh::AFogOfWarMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	vision_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("VisionMesh"));
	static ConstructorHelpers::FObjectFinder<UMaterial> FOWMat(TEXT("Material'/Game/Materials/FogOfWarInvis.FogOfWarInvis'"));
	fow_mat = FOWMat.Object;

	SetReplicates(true);
	
	vision_mesh->SetIsReplicated(true);
	vision_mesh->SetRenderCustomDepth(true);
	vision_mesh->SetCastShadow(false);
	vision_mesh->SetCustomDepthStencilValue(1);
	vision_mesh->SetRenderInMainPass(false);
	vision_mesh->bRenderInDepthPass = false;
	//vision_mesh->SetMaterial(0, fow_mat);
	TArray < FVector > Vertices;
	TArray < int32 > Triangles;
	TArray < FVector > Normals;
	int max_vision_rays = 100;
	Vertices.Add(GetActorLocation());
	for (int i = 0; i < max_vision_rays; ++i)
	{
		FHitResult out_hit;
		auto actor_loc = GetActorLocation();
		float angle = i * 2 * PI / (max_vision_rays - 2);
		FVector out_vec(cos(angle), sin(angle), 0);

		Vertices.Add(GetActorLocation() + out_vec * vision_radius);
		if (i != 0)
		{
			Triangles.Add(0);
			Triangles.Add(i);
			Triangles.Add(i - 1);
			Normals.Add(FVector(0, 0, 1));
		}
	}
	//Vertices.AddZeroed(101);
	//Normals.AddZeroed(99);
	TArray < FVector2D > UV0;
	TArray < FColor > VertexColors;
	TArray < FProcMeshTangent > Tangents;
	vision_mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);
	SetRootComponent(vision_mesh);
	SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void AFogOfWarMesh::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFogOfWarMesh::SetVisionMesh_Implementation(
	const TArray < FVector >& Vertices,
	const TArray < int32 >& Triangles,
	const TArray < FVector >& Normals
)
{
#if WITH_EDITOR
	ENetMode netMode = GetWorld()->GetNetMode();
	ENetRole netRole = GetLocalRole();
	ENetRole netRoleRemote = GetRemoteRole();
#endif

	TArray < FVector2D > UV0;
	TArray < FColor > VertexColors;
	TArray < FProcMeshTangent > Tangents;
	if (!vision_mesh->GetNumSections())
	{
		vision_mesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, false);
	}
	else
	{
		//if (HasAuthority())
		{
			vision_mesh->UpdateMeshSection(0, Vertices, Normals, UV0, VertexColors, Tangents);
			bool visible = vision_mesh->IsVisible();
			auto mat = vision_mesh->GetMaterial(0);
		}
		//else
		{
			printf("Whatttt");
		}
	}
}

void AFogOfWarMesh::Tick(float DeltaTime)
{
#if WITH_EDITOR
	ENetMode netMode = GetWorld()->GetNetMode();
	ENetRole netRole = GetLocalRole();
	ENetRole netRoleRemote = GetRemoteRole();
#endif
	if (netMode == NM_DedicatedServer || !vision_actor)
	{
		return;
	}
	Super::Tick(DeltaTime);
	TArray<AActor*> field_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UFieldActorInterface::StaticClass(), field_actors);

	auto vision_char = Cast<AAghsCloneCharacter>(vision_actor);
	vision_radius = 100;
	if (vision_char)
	{
		vision_radius = vision_char->GetVisionRadius();
	}

	FCollisionQueryParams ignore_me;
	//ignore_me.AddIgnoredActor(act);
	ignore_me.AddIgnoredActors(field_actors);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;

	if (GetWorld()->IsClient())
	{
		printf("IS CLIENT WHEN SHOULD BE CLIENT!!!!");
	}

	int max_vision_rays = 100;
	//auto vision_bounds = Cast<USphereComponent>(vision_actor->GetComponentsByTag(USphereComponent::StaticClass(), FName("));
	Vertices.Add(vision_actor->GetActorLocation());
	for (int i = 0; i < max_vision_rays; ++i)
	{
		FHitResult out_hit;
		auto actor_loc = vision_actor->GetActorLocation();
		float angle = i * 2 * PI / (max_vision_rays - 2);
		FVector out_vec(cos(angle), sin(angle), 0);

		GetWorld()->LineTraceSingleByChannel(out_hit,
			actor_loc,
			actor_loc + out_vec * vision_radius,
			ECollisionChannel::ECC_WorldDynamic,
			ignore_me);

		if (out_hit.bBlockingHit)
		{
			Vertices.Add(actor_loc + out_vec * (out_hit.Distance + 10));
			if (i != 0)
			{
				Triangles.Add(0);
				Triangles.Add(i);
				Triangles.Add(i - 1);
				Normals.Add(FVector(0, 0, 1));
			}
		}
		else
		{
			Vertices.Add(actor_loc + out_vec * vision_radius);
			if (i != 0)
			{
				Triangles.Add(0);
				Triangles.Add(i);
				Triangles.Add(i - 1);
				Normals.Add(FVector(0, 0, 1));
			}
		}
	}

	SetVisionMesh(Vertices, Triangles, Normals);
}
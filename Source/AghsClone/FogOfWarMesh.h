// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "ProceduralMeshComponent.h"
#include "FogOfWarMesh.generated.h"

UCLASS()
class AGHSCLONE_API AFogOfWarMesh : public AActor
{
	GENERATED_BODY()

	

	UMaterial* fow_mat;
	float vision_radius;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	AActor* vision_actor;
	
public:	
	// Sets default values for this actor's properties
	AFogOfWarMesh();

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UProceduralMeshComponent* vision_mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVisionRadius(float in_radius)
	{
		vision_radius = in_radius;
	}

	UFUNCTION(Reliable, Client)
	virtual void SetVisionMesh(
		const TArray < FVector >& Vertices,
		const TArray < int32 >& Triangles,
		const TArray < FVector >& Normals
	);
	/*
	UFUNCTION(Reliable, Client)
	void create_client_fow(AActor* act, USphereComponent* vision_bounds, const TArray<AActor*>& field_actors);
	*/
	void SetVisibility(bool IsVisible)
	{
		vision_mesh->SetVisibility(IsVisible);
	}
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(AFogOfWarMesh, vision_mesh);
		DOREPLIFETIME(AFogOfWarMesh, vision_actor);
	}

	void SetVisionActor(AActor* in_actor)
	{
		vision_actor = in_actor;
	}
};

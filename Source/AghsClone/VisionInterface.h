// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FogOfWarMesh.h"
#include "VisionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVisionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IVisionInterface
{
	GENERATED_BODY()

public:
	virtual float GetVisionRadius() const = 0;
	virtual void SetVisionMesh(
		const TArray < FVector >& Vertices,
		const TArray < int32 >& Triangles,
		const TArray < FVector >& Normals
	)
	{

	}

	virtual void SetVisionMesh(AFogOfWarMesh* in_mesh)
	{

	}
};

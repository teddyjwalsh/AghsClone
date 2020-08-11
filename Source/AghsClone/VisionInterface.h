// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
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
};

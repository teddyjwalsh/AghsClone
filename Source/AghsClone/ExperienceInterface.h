// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <map>

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExperienceInterface.generated.h"

extern std::vector<float> level_thresholds;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UExperienceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API IExperienceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GiveExperience(float in_exp) = 0;
	virtual float GetExperience() const = 0;
	virtual int32 GetLevel() const = 0;
};

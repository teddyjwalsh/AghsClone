// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AghsCloneCharacter.h"
#include "AttributeComponent.h"
#include "Hero.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API AHero : public AAghsCloneCharacter
{
	GENERATED_BODY()

	UAttributeComponent* attributes;

public:
	AHero();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AghsCloneCharacter.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API AEnemyCharacter : public AAghsCloneCharacter
{
	GENERATED_BODY()
	
	AEnemyCharacter()
	{
		SetBaseMovespeed(200);
		SetMaxHealth(200);
		SetMaxMana(75);
		SetArmor(0);
		SetAttackDamage(50);
		SetAttackProjectileSpeed(900);
		BaseAttackPoint = 0.3;
		BaseAttackBackswing = 0.2;
	}
};

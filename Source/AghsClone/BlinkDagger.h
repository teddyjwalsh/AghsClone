// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "NavigationSystem.h"
#include "AghsCloneCharacter.h"
#include "BlinkDagger.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API ABlinkDagger : public AItem
{
	GENERATED_BODY()
	
	ABlinkDagger()
	{
		bGroundTargeted = true;
		bActive = true;
	}

	virtual void OnGroundActivation(const FVector& ground_target) override
	{
		auto owner = Cast<AAghsCloneCharacter>(GetOwner());
		if (owner)
		{
			auto forward_vec = owner->GetActorForwardVector();
			forward_vec.Normalize();

			UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
			FNavLocation NavPoint;
			NavSys->ProjectPointToNavigation(ground_target, NavPoint);

			owner->SetActorLocation(NavPoint.Location);
		}
	}
};

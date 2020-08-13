// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "AghsClonePlayerController.h"
#include "AghsCloneGameMode.generated.h"

UCLASS(minimalapi)
class AAghsCloneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAghsCloneGameMode();

	virtual void PostLogin(APlayerController * NewPlayer) override
	{
		Super::PostLogin(NewPlayer);
		auto aghs_cont = Cast<AAghsClonePlayerController>(NewPlayer);
		UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
		if (aghs_cont)
		{
			FNavLocation NavPoint;
			NavSys->ProjectPointToNavigation(FVector(0, 0, 0), NavPoint);
			//auto new_character = GetWorld()->SpawnActor<AAghsCloneCharacter>(NavPoint.Location, FRotator());
			AAghsCloneCharacter* new_character = Cast< AAghsCloneCharacter>(UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), AAghsCloneCharacter::StaticClass(), nullptr, NavPoint.Location));
			new_character->SetTeam(GetNumPlayers());
			new_character->SetUnitOwner(aghs_cont->GetPawn());
			//NavSys->ProjectPointToNavigation(FVector(0,0,0), FNav)
			aghs_cont->team = GetNumPlayers();
			aghs_cont->AssignTeam(GetNumPlayers());
		}
	}
};




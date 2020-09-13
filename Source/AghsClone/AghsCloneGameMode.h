// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"

#include <unordered_map>

#include "AghsClonePlayerController.h"
#include "CrystalMaidenHero.h"
#include "Hero.h"
#include "AghsCloneGameMode.generated.h"


UCLASS(minimalapi)
class AAghsCloneGameMode : public AGameModeBase
{

	GENERATED_BODY()
	void StartPlay() override;
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
			NavSys->ProjectPointToNavigation(FVector(-1450, -1550, 0), NavPoint);
			//auto new_character = GetWorld()->SpawnActor<AAghsCloneCharacter>(NavPoint.Location, FRotator());
			AAghsCloneCharacter* new_character = Cast<AHero>(UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), ACrystalMaidenHero::StaticClass(), nullptr, NavPoint.Location));
			new_character->SetTeam(GetNumPlayers());
			new_character->SetUnitOwner(aghs_cont->GetPawn());
			TArray<AAghsCloneCharacter*> init_select;
			init_select.Add(new_character);
			//NavSys->ProjectPointToNavigation(FVector(0,0,0), FNav)
			
			aghs_cont->team = GetNumPlayers();
			aghs_cont->AssignTeam(GetNumPlayers());
			aghs_cont->SetSelected(init_select);
		}
	}
};




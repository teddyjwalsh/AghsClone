// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
		if (aghs_cont)
		{
			aghs_cont->team = GetNumPlayers();
			aghs_cont->AssignTeam(GetNumPlayers());
		}
	}
};




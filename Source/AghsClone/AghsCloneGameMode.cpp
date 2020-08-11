// Copyright Epic Games, Inc. All Rights Reserved.

#include "AghsCloneGameMode.h"
#include "AghsClonePlayerController.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "UObject/ConstructorHelpers.h"
#include "InGameHud.h"
#include "GameFramework/GameUserSettings.h"

AAghsCloneGameMode::AAghsCloneGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAghsClonePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	DefaultPawnClass = AUnitController::StaticClass();
	HUDClass = AInGameHud::StaticClass();

	if (GEngine)
	{
		UGameUserSettings* MyGameSettings = GEngine->GetGameUserSettings();
		//GEngine->GameViewport->SetCaptureMouseOnClick(EMouseCaptureMode::CapturePermanently);
		MyGameSettings->SetScreenResolution(FIntPoint(1024, 768));
		MyGameSettings->SetFullscreenMode(EWindowMode::Windowed);
		MyGameSettings->SetVSyncEnabled(true);
		MyGameSettings->ApplySettings(true);
	}
}
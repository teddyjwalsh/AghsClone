// Copyright Epic Games, Inc. All Rights Reserved.

#include "AghsCloneGameMode.h"
#include "AghsClonePlayerController.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "UObject/ConstructorHelpers.h"
#include "InGameHud.h"
#include "Shop.h"
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
	//ReplicationDriver::CreateReplicationDriverDelegate().BindLambda([](UNetDriver* ForNetDriver, const FURL& URL, UWorld* World) -> UReplicationDriver*
	//		{
	//		return NewObject<UAghsReplicationDriver>(GetTransientPackage());
	//	});
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

void AAghsCloneGameMode::StartPlay()
{
	Super::StartPlay();

	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	FNavLocation NavPoint;
	NavSys->ProjectPointToNavigation(FVector(0, 0, 0), NavPoint);
	auto shop = AShop::CreateShop(GetWorld(), NavPoint.Location);

	auto new_item = GetWorld()->SpawnActor<AItem>();
	new_item->Health = 125;
	new_item->Mana = 200;
	shop->AddItem(new_item, 1, 500);

	new_item = GetWorld()->SpawnActor<AItem>();
	new_item->AttackSpeed = 100;
	new_item->AttackDamage = 50;
	shop->AddItem(new_item, 2, 200);
}
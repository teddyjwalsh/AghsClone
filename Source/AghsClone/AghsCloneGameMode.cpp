// Copyright Epic Games, Inc. All Rights Reserved.

#include "AghsCloneGameMode.h"
#include "AghsClonePlayerController.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "UObject/ConstructorHelpers.h"
#include "InGameHud.h"
#include "Shop.h"
#include "BlinkDagger.h"
#include "GameFramework/GameUserSettings.h"
#include "GameMapsSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

TMap<FString, UTexture2D*> FCharacterSpec::textures;

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
		bUseSeamlessTravel = true;
	}
	
}

void AAghsCloneGameMode::StartPlay()
{
	Super::StartPlay();
	AShop::DestroyShops();

	UNavigationSystemV1* NavSys = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	FNavLocation NavPoint;
	NavSys->ProjectPointToNavigation(FVector(0, 0, 0), NavPoint);
	auto shop = AShop::CreateShop(GetWorld(), NavPoint.Location);

	auto new_item = GetWorld()->SpawnActor<AItem>();
	new_item->Health = 125;
	new_item->Mana = 200;
	new_item->SetMaterial("kotlguyp");
	shop->AddItem(new_item, 1, 500);

	new_item = GetWorld()->SpawnActor<AItem>();
	new_item->AttackSpeed = 140;
	new_item->AttackDamage = 0;
	new_item->SetMaterial("moon_shard");
	shop->AddItem(new_item, 2, 200);

	new_item = GetWorld()->SpawnActor<AItem>();
	new_item->Movespeed = 45;
	new_item->SetMaterial("brown_boots");
	shop->AddItem(new_item, 3, 500);

	new_item = GetWorld()->SpawnActor<AItem>();
	new_item->AttackSpeed = 1000;
	new_item->AttackDamage = -50;
	new_item->SetMaterial("serpentblade");
	shop->AddItem(new_item, 4, 123);

	new_item = GetWorld()->SpawnActor<ABlinkDagger>();
	new_item->CastRange = 1200;
	new_item->SetMaterial("blink_dagger");
	shop->AddItem(new_item, 5, 800);
}
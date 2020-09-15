// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "SessionMenuInterface.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UMainMenu : public UUserWidget
{
	
	GENERATED_BODY()
		
	UPROPERTY(meta = (BindWidget))
	UButton* NewSessionButton;
	UButton* JoinSessionButton;

	ISessionMenuInterface* SessionMenuInterface;
	
	bool Initialize()
	{
		NewSessionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), "Grid");
		JoinSessionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), "Grid");
		NewSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnNewSessionPressed);
		JoinSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinSessionPressed);
		return true;
	}
	
	void OnNewSessionPressed() 
	{
		if (SessionMenuInterface == nullptr)
		{
			return;
		}
		SessionMenuInterface->Host("CoopPuzzleGameServer");
	}

	void OnJoinSessionPressed()
	{
		if (SessionMenuInterface == nullptr)
		{
			return;
		}
		SessionMenuInterface->JoinSession(1);
	}

};

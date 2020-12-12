// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "SessionMenuInterface.h"
#include "Kismet/GameplayStatics.h"
#include "MainMenu.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGHSCLONE_API UMainMenu : public UUserWidget
{
public:
	GENERATED_BODY()
		
	UPROPERTY(meta = (BindWidget))
	UButton* NewSessionButton;
	UButton* JoinSessionButton;
	UUniformGridPanel* grid;

	ISessionMenuInterface* SessionMenuInterface;
	
	bool Initialize()
	{
		Super::Initialize();
		SessionMenuInterface = Cast<ISessionMenuInterface>(UGameplayStatics::GetGameInstance(GetWorld()));
		grid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), "MainMenuGrid");
		WidgetTree->RootWidget = grid;
		NewSessionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), "NewSessionButton");

		auto widget_text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), "NewSessionText");
		widget_text->SetText(FText::FromString("New Session"));
		NewSessionButton->AddChild(widget_text);

		grid->AddChildToUniformGrid(NewSessionButton, 0, 1);

		JoinSessionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), "JoinSessionButton");

		auto widget_text2 = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), "JoinSessionText");
		widget_text2->SetText(FText::FromString("Join Session"));
		JoinSessionButton->AddChild(widget_text2);

		grid->AddChildToUniformGrid(JoinSessionButton, 0, 2);
		NewSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnNewSessionPressed);
		JoinSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinSessionPressed);
		return true;
	}
	
	UFUNCTION()
	void OnNewSessionPressed() 
	{
		if (SessionMenuInterface == nullptr)
		{
			return;
		}
		SessionMenuInterface->Host("CoopPuzzleGameServer");
		UGameplayStatics::OpenLevel(GetWorld(), FName("GameTestMap"));
	}

	UFUNCTION()
	void OnJoinSessionPressed()
	{
		if (SessionMenuInterface == nullptr)
		{
			return;
		}
		SessionMenuInterface->JoinSession(1);
	}

};

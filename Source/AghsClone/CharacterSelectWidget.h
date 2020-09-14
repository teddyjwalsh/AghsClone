// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/UniformGridSlot.h"
#include <functional>
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"
#include "InputCoreTypes.h"
#include "StoreWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/Spacer.h"

#include "StoreWidget.h"
#include "GameModeInfo.h"
#include "CharacterSelectWidget.generated.h"
#pragma once

UCLASS()
class AGHSCLONE_API UCharacterSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UGridPanel* grid;
	FCharacterSpec* character;
	UMultiButton* ability_button;
	UOverlay* overlay;
	UTextBlock* level;
	UTextBlock* cooldown;
	virtual bool Initialize() override;

	bool Refresh();

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);
};

UCLASS()
class AGHSCLONE_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UUniformGridPanel* grid;
	TMap<UCharacterSelectionWidget*, FCharacterSpec*> buttons;
	TArray<FCharacterSpec> character_list;
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

	void SetAbilities();
	bool Refresh();

	bool GridIsHovered() const
	{
		return grid->IsHovered();
	}
};


/**
 * 

UCLASS()
class AGHSCLONE_API UCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	TArray<FCharacterSpec> character_list;
	UMultiButton* button;
	UUniformGridPanel* grid;
	//TArray<UMultiButton*> buttons;
	TMap<UMultiButton*, ItemId> buttons;

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);

	UFUNCTION()
	void OnItemHovered(UMultiButton* in_button);

	bool GridIsHovered() const;
};
 */
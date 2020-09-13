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
#include "GameModeInfo.h"
#include "CharacterSelectWidget.generated.h"


/**
 * 
 */
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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include <functional>
#include "Components/ButtonSlot.h"
#include "Blueprint/WidgetTree.h"
#include "InputCoreTypes.h"

#include "Shop.h"
#include "InventoryComponent.h"
#include "Ability.h"

#include "StoreWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadDelegate, UMultiButton*, in_button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

UCLASS()
class AGHSCLONE_API UMultiButton : public UBorder
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnPointerEvent OnRightMouseButtonDownEvent;

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
	FOnPointerEvent OnLeftMouseButtonDownEvent;

	UImage* image;

public:
	UMultiButton()
	{
		OnRightMouseButtonDownEvent.BindUFunction(this, "OnRightClick");
		OnLeftMouseButtonDownEvent.BindUFunction(this, "OnLeftClick");
		//auto image_widget = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), "Grid");
		//SetBrushColor(FLinearColor(1, 0, 0));
	}

    UPROPERTY()
		FLoadDelegate RightClick;
		FLoadDelegate LeftClick;
		FLoadDelegate hover;

    UPROPERTY()
        FClickDelegate click;

	UFUNCTION()
	void OnRightClick()
	{
		RightClick.Broadcast(this);
	}

	UFUNCTION()
	void OnLeftClick()
	{
		LeftClick.Broadcast(this);
	}

	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
	{
		if (OnRightMouseButtonDownEvent.IsBound() && MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			return OnRightMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
		}
		else if (OnLeftMouseButtonDownEvent.IsBound() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			return OnLeftMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
		}

		return FReply::Unhandled();
	}


	virtual void SynchronizeProperties() override
	{
		Super::Super::SynchronizeProperties();

		TAttribute<FLinearColor> ContentColorAndOpacityBinding = PROPERTY_BINDING(FLinearColor, ContentColorAndOpacity);
		TAttribute<FSlateColor> BrushColorBinding = OPTIONAL_BINDING_CONVERT(FLinearColor, BrushColor, FSlateColor, ConvertLinearColorToSlateColor);
		TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Background, const FSlateBrush*, ConvertImage);

		MyBorder->SetPadding(Padding);
		MyBorder->SetBorderBackgroundColor(BrushColorBinding);
		MyBorder->SetColorAndOpacity(ContentColorAndOpacityBinding);

		MyBorder->SetBorderImage(ImageBinding);

		MyBorder->SetDesiredSizeScale(DesiredSizeScale);
		MyBorder->SetShowEffectWhenDisabled(bShowEffectWhenDisabled != 0);

		MyBorder->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
		MyBorder->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
		MyBorder->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
		MyBorder->SetOnMouseDoubleClick(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseDoubleClick));
	}
};

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UStoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:
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

/**
 * 
 */
UCLASS()
class AGHSCLONE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UMultiButton* button;
	UUniformGridPanel* grid;
	//TArray<UMultiButton*> buttons;
	TMap<UMultiButton*, AItem*> buttons;
    UInventoryComponent* current_inventory = nullptr;
	virtual bool Initialize() override;
    bool DrawInventory();

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);

	UFUNCTION()
	void OnItemHovered(UMultiButton* in_button);

	void SetItems();

	bool GridIsHovered() const;
};

UCLASS()
class AGHSCLONE_API UAbilitiesWidget: public UUserWidget
{
	GENERATED_BODY()
public:
	int32 el_height;
	int32 el_width;
	UMultiButton* button;
	UUniformGridPanel* grid;
	//TArray<UMultiButton*> buttons;
	TMap<UMultiButton*, UAbility*> buttons;
    TArray<UAbility*> abilities;
	virtual bool Initialize() override;

    bool DrawAbilities();

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);

	UFUNCTION()
	void OnItemHovered(UMultiButton* in_button);

	void SetAbilities();

	bool GridIsHovered() const;
};

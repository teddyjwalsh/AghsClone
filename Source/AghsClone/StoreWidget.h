// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include <functional>
#include "StoreWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadDelegate, UMultiButton*, in_button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class SLATE_API SButtonRight
	: public SButton
{
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
	{
		FReply Reply = FReply::Unhandled();
		if (IsEnabled() && (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton || MouseEvent.IsTouchEvent()))
		{
			Press();
			PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

			EButtonClickMethod::Type InputClickMethod = GetClickMethodFromInputType(MouseEvent);

			if (InputClickMethod == EButtonClickMethod::MouseDown)
			{
				//get the reply from the execute function
				Reply = ExecuteOnClick();

				//You should ALWAYS handle the OnClicked event.
				ensure(Reply.IsEventHandled() == true);
			}
			else if (InputClickMethod == EButtonClickMethod::PreciseClick)
			{
				// do not capture the pointer for precise taps or clicks
				// 
				Reply = FReply::Handled();
			}
			else
			{
				//we need to capture the mouse for MouseUp events
				Reply = FReply::Handled().CaptureMouse(AsShared());
			}
		}

		Invalidate(EInvalidateWidget::Layout);

		//return the constructed reply
		return Reply;
	}
};

UCLASS()
class AGHSCLONE_API UMultiButton : public UButton
{
    GENERATED_BODY()

public:
	UMultiButton()
	{
		OnClicked.AddDynamic(this, &UMultiButton::OnClick);
	}

    UPROPERTY()
        FLoadDelegate load;

    UPROPERTY()
        FClickDelegate click;

	UFUNCTION()
	void OnClick()
	{
		load.Broadcast(this);
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
	TArray<UMultiButton*> buttons;
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	FClickDelegate click_delegate;

	std::function<void(void)> on_item_click;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);
};

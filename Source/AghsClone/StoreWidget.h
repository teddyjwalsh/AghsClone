// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include <functional>
#include "Components/ButtonSlot.h"
#include "Shop.h"
#include "InputCoreTypes.h"

#include "StoreWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadDelegate, UMultiButton*, in_button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClickDelegate);

class SLATE_API SButtonRight
	: public SButton
{
	SLATE_BEGIN_ARGS(SButtonRight)
		: _Content()
		, _ButtonStyle(&FCoreStyle::Get().GetWidgetStyle< FButtonStyle >("ButtonRight"))
		, _TextStyle(&FCoreStyle::Get().GetWidgetStyle< FTextBlockStyle >("NormalText"))
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _ContentPadding(FMargin(4.0, 2.0))
		, _Text()
		, _ClickMethod(EButtonClickMethod::DownAndUp)
		, _TouchMethod(EButtonTouchMethod::DownAndUp)
		, _PressMethod(EButtonPressMethod::DownAndUp)
		, _DesiredSizeScale(FVector2D(1, 1))
		, _ContentScale(FVector2D(1, 1))
		, _ButtonColorAndOpacity(FLinearColor::White)
		, _ForegroundColor(FCoreStyle::Get().GetSlateColor("InvertedForeground"))
		, _IsFocusable(true)
	{
	}

	/** Slot for this button's content (optional) */
	SLATE_DEFAULT_SLOT(FArguments, Content)

		/** The visual style of the button */
		SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)

		/** The text style of the button */
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

		/** Horizontal alignment */
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)

		/** Vertical alignment */
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)

		/** Spacing between button's border and the content. */
		SLATE_ATTRIBUTE(FMargin, ContentPadding)

		/** The text to display in this button, if no custom content is specified */
		SLATE_ATTRIBUTE(FText, Text)

		/** Called when the button is clicked */
		SLATE_EVENT(FOnClicked, OnClicked)

		/** Called when the button is pressed */
		SLATE_EVENT(FSimpleDelegate, OnPressed)

		/** Called when the button is released */
		SLATE_EVENT(FSimpleDelegate, OnReleased)

		SLATE_EVENT(FSimpleDelegate, OnHovered)

		SLATE_EVENT(FSimpleDelegate, OnUnhovered)

		/** Sets the rules to use for determining whether the button was clicked.  This is an advanced setting and generally should be left as the default. */
		SLATE_ARGUMENT(EButtonClickMethod::Type, ClickMethod)

		/** How should the button be clicked with touch events? */
		SLATE_ARGUMENT(EButtonTouchMethod::Type, TouchMethod)

		/** How should the button be clicked with keyboard/controller button events? */
		SLATE_ARGUMENT(EButtonPressMethod::Type, PressMethod)

		SLATE_ATTRIBUTE(FVector2D, DesiredSizeScale)

		SLATE_ATTRIBUTE(FVector2D, ContentScale)

		SLATE_ATTRIBUTE(FSlateColor, ButtonColorAndOpacity)

		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)

		/** Sometimes a button should only be mouse-clickable and never keyboard focusable. */
		SLATE_ARGUMENT(bool, IsFocusable)

		/** The sound to play when the button is pressed */
		SLATE_ARGUMENT(TOptional<FSlateSound>, PressedSoundOverride)

		/** The sound to play when the button is hovered */
		SLATE_ARGUMENT(TOptional<FSlateSound>, HoveredSoundOverride)

		/** Which text shaping method should we use? (unset to use the default returned by GetDefaultTextShapingMethod) */
		SLATE_ARGUMENT(TOptional<ETextShapingMethod>, TextShapingMethod)

		/** Which text flow direction should we use? (unset to use the default returned by GetDefaultTextFlowDirection) */
		SLATE_ARGUMENT(TOptional<ETextFlowDirection>, TextFlowDirection)

		SLATE_END_ARGS()

public:
	SButtonRight():
		SButton()
	{
	}

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
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
		OnHovered.AddDynamic(this, &UMultiButton::OnHover);
	}

    UPROPERTY()
		FLoadDelegate load;
		FLoadDelegate hover;

    UPROPERTY()
        FClickDelegate click;

	UFUNCTION()
	void OnClick()
	{
		load.Broadcast(this);
	}

	UFUNCTION()
	void OnHover()
	{
		hover.Broadcast(this);
	}
/*
	virtual TSharedRef<SWidget> RebuildWidget() override
	{
		
		TSharedPtr<SButtonRight> my_right_button = SNew(SButtonRight)
			.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
			.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
			.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
			.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
			.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
			.ButtonStyle(&WidgetStyle)
			.ClickMethod(ClickMethod)
			.TouchMethod(TouchMethod)
			.PressMethod(PressMethod)
			.IsFocusable(IsFocusable)
			;
		MyButton = my_right_button;
		if (GetChildrenCount() > 0)
		{
			//Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
		}

		return MyButton.ToSharedRef();
	}
*/
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

	FClickDelegate click_delegate;

	std::function<void(void)> on_item_click;

	UFUNCTION()
	void OnItemClicked(UMultiButton* in_button);

	UFUNCTION()
	void OnItemHovered(UMultiButton* in_button);
};

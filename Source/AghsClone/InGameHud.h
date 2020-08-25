// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "AghsCloneCharacter.h"
#include "AghsClonePlayerController.h"
#include "InventoryComponent.h"
#include "Components/Button.h"
#include "StoreWidget.h"
#include "InGameHud.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API AInGameHud : public AHUD
{
	GENERATED_BODY()

	TArray<AAghsCloneCharacter*> selected;
	TArray<UButton*> buttons;

public:
	AInGameHud()
	{
		
	}

	virtual void BeginPlay() override
	{
		//auto UserInterface = CreateWidget<UStoreWidget>(this, UStoreWidget::StaticClass());
		//FInputModeGameAndUI Mode;
		//Mode.SetLockMouseToViewportBehavior(EMouseLockMode::true);
		//Mode.SetHideCursorDuringCapture(false);
		//SetInputMode(Mode);
		//UserInterface->AddToViewport(9999); // Z-order, this just makes it render on the very top.
	}

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override
	{
		EnableInput(GetWorld()->GetFirstPlayerController());
		
		Super::DrawHUD();
		auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstPlayerController());

		auto pawn = pc->GetPawn();
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem health_text(FVector2D(0, 0), FText(), def_font, FLinearColor(0, 0, 0));
		FString health_string;
		if (pawn)
		{
			health_string = FString::Printf(TEXT("%f / %f / %f"),
				pawn->GetActorLocation().X,
				pawn->GetActorLocation().Y,
				pawn->GetActorLocation().Z);
			health_text.Text = FText::FromString(health_string);
			health_text.Position = FVector2D(100, 100);
			Canvas->DrawItem(health_text);
		}
		
		if (pc != nullptr)
		{
			float mx, my;
			pc->GetMousePosition(mx, my);
			UpdateAndDispatchHitBoxClickEvents(FVector2D(mx, my), EInputEvent::IE_Pressed);
			// Health bars
			int32 vx, vy;
			pc->GetViewportSize(vx, vy);
			TArray<AAghsCloneCharacter*> screen_chars;
			GetActorsInSelectionRectangle(FVector2D(0,0), FVector2D(vx, vy), screen_chars, false);
			
			// Draw health bars above units
			for (auto& sc : screen_chars)
			{
				if (sc->IsHidden())
				{
					continue;
				}
				auto char_health_interface = Cast<IHealthInterface>(sc);
				if (char_health_interface)
				{
					FVector2D actor_screen_loc;
					if (pc->ProjectWorldLocationToScreen(sc->GetActorLocation(), actor_screen_loc))
					{
						FVector health_color;
						if (sc->GetTeam() == pc->GetTeam())
						{
							health_color = FVector(0, 1, 0);
						}
						else
						{
							health_color = FVector(1, 0, 0);
						}
						DrawUnitBar(char_health_interface, actor_screen_loc, health_color);
					}
				}
			}

			// Draw main health bar
			int32 x_center = vx / 2;
			//TArray<AAghsCloneCharacter*> selected_chars;
			auto& selected_chars = pc->GetSelected();
			for (auto& sc : selected_chars)
			{
				auto char_health_interface = Cast<IHealthInterface>(sc);
				if (char_health_interface)
				{
					DrawHealthBar(char_health_interface, FVector2D(x_center, vy - 32));
					auto char_mana_interface = Cast<IManaInterface>(sc);
					if (char_mana_interface)
					{
						DrawManaBar(char_mana_interface, FVector2D(x_center, vy - 15));
					}
					auto wallet_comp = Cast<UWalletComponent>(sc->GetComponentByClass(UWalletComponent::StaticClass()));
					if (wallet_comp)
					{
						DrawWallet(wallet_comp, FVector2D(vx - 120, vy - 100));
					}
					auto xp_interface = Cast<IExperienceInterface>(sc);
					if (xp_interface)
					{
						DrawLevel(xp_interface, FVector2D(vx - 120, vy - 150));
					}
                    IAbilityInterface* channeled = sc->GetChanneled();
                    if (channeled)
                    {
                        float current = channeled->GetChannelTime();
                        float max = channeled->GetMaxChannelTime();
                        DrawChannelBar(current, max, FVector2D(vx/2, vy/2));
                    }

					break;
				}
			}

			if (pc->SelectBoxOn())
			{
				pc->GetSelectBox(box_start, box_end);
				box_loc = FVector2D(std::min(box_start.X, box_end.X), std::min(box_start.Y, box_end.Y));
				box_size = FVector2D(std::max(box_start.X, box_end.X), std::max(box_start.Y, box_end.Y)) - box_loc;
				FCanvasBoxItem box_item(box_loc, box_size);
				Canvas->DrawItem(box_item);
				select_was_on = true;
			}
			else if (!pc->SelectBoxOn() && select_was_on)
			{
				select_was_on = false;
				TArray<AAghsCloneCharacter*> out_chars;
				if ((box_start - box_end).Size() > 5)
				{
					GetActorsInSelectionRectangle(box_start, box_end, out_chars, false);
					out_chars.Remove(nullptr);
					for (auto& act : selected)
					{
						act->SetSelected(false);
					}
					selected = out_chars;
					for (auto& act : selected)
					{
						act->SetSelected(true);
					}
					pc->SetSelected(out_chars);
				}
				UE_LOG(LogTemp, Warning, TEXT("CHARACTERS"));
			}
			
		}
		
	}
	
	void DrawUnitBar(IHealthInterface* char_health_interface, FVector2D screen_loc, const FVector& in_color)
	{
			FVector2D health_box_size(40, 5);
			FVector2D health_box_loc = screen_loc + FVector2D(-20, -30);
			FCanvasBoxItem box_item(health_box_loc - FVector2D(0, 0), health_box_size + FVector2D(1, 1));
			float health_fraction = char_health_interface->GetHealth() * 1.0 / char_health_interface->GetMaxHealth();
			FCanvasTileItem tile_item(health_box_loc, health_box_size * FVector2D(health_fraction, 1), in_color);
			box_item.SetColor(FLinearColor(1, 1, 1));
			Canvas->DrawItem(tile_item);
			Canvas->DrawItem(box_item);
	}

	void DrawHealthBar(IHealthInterface* char_health_interface, FVector2D screen_loc)
	{
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem health_text(FVector2D(0, 0), FText(), def_font, FLinearColor(1, 1, 1));

		FVector2D main_health_box_size(400, 15);
		FVector2D main_health_box_loc = FVector2D(screen_loc.X - main_health_box_size.X / 2,  screen_loc.Y);
		FCanvasBoxItem box_item(main_health_box_loc - FVector2D(0, 0), main_health_box_size + FVector2D(1, 1));
		float cur_health = char_health_interface->GetHealth();
		float max_health = char_health_interface->GetMaxHealth();
		float health_fraction = cur_health * 1.0 / max_health;
		FString health_string;
		health_string = FString::Printf(TEXT("%d / %d"),
			int32(char_health_interface->GetHealth()),
			int32(char_health_interface->GetMaxHealth()));
		health_text.Text = FText::FromString(health_string);
		health_text.Position = FVector2D(screen_loc.X - health_text.DrawnSize.X / 2, main_health_box_loc.Y);

		FCanvasTileItem tile_item(main_health_box_loc, main_health_box_size * FVector2D(health_fraction, 1), FLinearColor(1, 0, 0));
		box_item.SetColor(FLinearColor(1, 1, 1));
		Canvas->DrawItem(tile_item);
		Canvas->DrawItem(box_item);
		Canvas->DrawItem(health_text);
	}

	void DrawManaBar(IManaInterface* char_mana_interface, FVector2D screen_loc)
	{
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem health_text(FVector2D(0, 0), FText(), def_font, FLinearColor(1, 1, 1));

		FVector2D main_health_box_size(400, 15);
		FVector2D main_health_box_loc = FVector2D(screen_loc.X - main_health_box_size.X / 2, screen_loc.Y);
		FCanvasBoxItem box_item(main_health_box_loc - FVector2D(0, 0), main_health_box_size + FVector2D(1, 1));
		float health_fraction = char_mana_interface->GetMana() * 1.0 / char_mana_interface->GetMaxMana();
		FString health_string;
		health_string = FString::Printf(TEXT("%d / %d"),
			int32(char_mana_interface->GetMana()),
			int32(char_mana_interface->GetMaxMana()));
		health_text.Text = FText::FromString(health_string);
		health_text.Position = FVector2D(screen_loc.X - health_text.DrawnSize.X / 2, main_health_box_loc.Y);

		FCanvasTileItem tile_item(main_health_box_loc, main_health_box_size * FVector2D(health_fraction, 1), FLinearColor(0, 0, 1));
		box_item.SetColor(FLinearColor(1, 1, 1));
		Canvas->DrawItem(tile_item);
		Canvas->DrawItem(box_item);
		Canvas->DrawItem(health_text);
	}

	void DrawWallet(UWalletComponent* wallet_comp, FVector2D screen_loc)
	{
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem health_text(FVector2D(0, 0), FText(), def_font, FLinearColor(1, 1, 1));
		FString health_string;
		health_string = FString::Printf(TEXT("%d gold"), int32(wallet_comp->Balance()));
		health_text.Text = FText::FromString(health_string);
		health_text.Position = FVector2D(screen_loc.X, screen_loc.Y);
		Canvas->DrawItem(health_text);
	}

	void DrawLevel(IExperienceInterface* xp_int, FVector2D screen_loc)
	{
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem level_text(FVector2D(0, 0), FText(), def_font, FLinearColor(1, 1, 1));
		FString level_string;
		level_string = FString::Printf(TEXT("Level: %d"), int32(xp_int->GetLevel()));
		level_text.Text = FText::FromString(level_string);
		level_text.Position = FVector2D(screen_loc.X, screen_loc.Y);
		Canvas->DrawItem(level_text);
	}

    void DrawChannelBar(float CurrentTime, float MaxTime, FVector2D screen_loc)
    {
		static auto def_font = GetFontFromSizeIndex(10);
		static FCanvasTextItem health_text(FVector2D(0, 0), FText(), def_font, FLinearColor(1, 1, 1));

		FVector2D main_health_box_size(200, 10);
		FVector2D main_health_box_loc = FVector2D(screen_loc.X - main_health_box_size.X / 2, screen_loc.Y);
		FCanvasBoxItem box_item(main_health_box_loc - FVector2D(0, 0), main_health_box_size + FVector2D(1, 1));
		float health_fraction = CurrentTime * 1.0 / MaxTime;
		FString health_string;
		health_string = FString::Printf(TEXT("%d / %d"),
			int32(CurrentTime),
			int32(MaxTime));
		health_text.Text = FText::FromString(health_string);
		health_text.Position = FVector2D(screen_loc.X - health_text.DrawnSize.X / 2, main_health_box_loc.Y);

		FCanvasTileItem tile_item(main_health_box_loc, main_health_box_size * FVector2D(health_fraction, 1), FLinearColor(0.3, 0.3, 0.3));
		box_item.SetColor(FLinearColor(1, 1, 1));
		Canvas->DrawItem(tile_item);
		Canvas->DrawItem(box_item);
		Canvas->DrawItem(health_text);
    }

	//virtual void BeginPlay() override
	//{
	//	Super::BeginPlay();
	//}

private:

	bool select_was_on;
	FVector2D box_loc, box_size;
	FVector2D box_start, box_end;
	float item_slot_width = 25;
	float item_slot_height = 25;
	float between_space = 2;
	/** Crosshair asset pointer */
	//class UTexture2D* CrosshairTex;

	//UPROPERTY(EditAnywhere, Category = "Health")
	//TSubclassOf<class UUserWidget> HUDWidgetClass;

	//UPROPERTY(EditAnywhere, Category = "Health")
	//class UUserWidget* CurrentWidget;
};

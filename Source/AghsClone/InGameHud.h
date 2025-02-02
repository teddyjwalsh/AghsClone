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
#include "InGameHud.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API AInGameHud : public AHUD
{
	GENERATED_BODY()

	TArray<AAghsCloneCharacter*> selected;

public:
	//AInGameHud()
	//{
		
	//}

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override
	{
		Super::DrawHUD();

		auto pc = Cast<AAghsClonePlayerController>(GetWorld()->GetFirstPlayerController());

		if (pc != nullptr)
		{
			// Health bars
			int32 vx, vy;
			pc->GetViewportSize(vx, vy);
			TArray<AAghsCloneCharacter*> screen_chars;
			GetActorsInSelectionRectangle(FVector2D(0,0), FVector2D(vx, vy), screen_chars, false);

			// Draw health bars above units
			for (auto& sc : screen_chars)
			{
				auto char_health_interface = Cast<IHealthInterface>(sc);
				if (char_health_interface)
				{
					FVector2D actor_screen_loc;
					if (pc->ProjectWorldLocationToScreen(sc->GetActorLocation(), actor_screen_loc))
					{
						DrawUnitBar(char_health_interface, actor_screen_loc);
					}
				}
			}

			// Draw main health bar
			int32 x_center = vx / 2;
			//TArray<AAghsCloneCharacter*> selected_chars;
			//pc->GetSelected(selected_chars);
			for (auto& sc : selected)
			{
				auto char_health_interface = Cast<IHealthInterface>(sc);
				if (char_health_interface)
				{
					DrawHealthBar(char_health_interface, FVector2D(x_center, vy - 100));
					auto char_mana_interface = Cast<IManaInterface>(sc);
					if (char_mana_interface)
					{
						DrawManaBar(char_mana_interface, FVector2D(x_center, vy - 83));
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
					selected = out_chars;
					pc->SetSelected(out_chars);
				}
				UE_LOG(LogTemp, Warning, TEXT("CHARACTERS"));
			}
		}
		
	}
	
	void DrawUnitBar(IHealthInterface* char_health_interface, FVector2D screen_loc)
	{
			FVector2D health_box_size(40, 5);
			FVector2D health_box_loc = screen_loc + FVector2D(-20, -30);
			FCanvasBoxItem box_item(health_box_loc - FVector2D(1, 1), health_box_size + FVector2D(2, 2));
			float health_fraction = char_health_interface->GetHealth() * 1.0 / char_health_interface->GetMaxHealth();
			FCanvasTileItem tile_item(health_box_loc, health_box_size * FVector2D(health_fraction, 1), FLinearColor(1, 0, 0));
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
		FCanvasBoxItem box_item(main_health_box_loc - FVector2D(1, 1), main_health_box_size + FVector2D(2, 2));
		float health_fraction = char_health_interface->GetHealth() * 1.0 / char_health_interface->GetMaxHealth();
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
		FCanvasBoxItem box_item(main_health_box_loc - FVector2D(1, 1), main_health_box_size + FVector2D(2, 2));
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


	//virtual void BeginPlay() override
	//{
	//	Super::BeginPlay();
	//}

private:

	bool select_was_on;
	FVector2D box_loc, box_size;
	FVector2D box_start, box_end;
	/** Crosshair asset pointer */
	//class UTexture2D* CrosshairTex;

	//UPROPERTY(EditAnywhere, Category = "Health")
	//TSubclassOf<class UUserWidget> HUDWidgetClass;

	//UPROPERTY(EditAnywhere, Category = "Health")
	//class UUserWidget* CurrentWidget;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "Ability.h"
#include "StoreWidget.h"
#include "AbilityContainerInterface.h"
#include "AbilityInterface.h"
#include "AghsClonePlayerController.generated.h"

UCLASS()
class AAghsClonePlayerController : public APlayerController
{
	GENERATED_BODY()

	float mx, my;
	bool select_box_on;
	
	

	FVector2D select_box_start;
	FVector2D select_box_end;
	IAbilityInterface* targeted_ability;
	int32 targeted_ability_num;
	TArray<AAghsCloneCharacter*> selected_units;
	TArray<AAghsCloneCharacter*> temp_units;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
	TSubclassOf<class UStoreWidget> StoreWidgetClass;
	UStoreWidget* StoreWidget;

	int32 team;

	AAghsClonePlayerController();

	UFUNCTION(Reliable, Client)
	void AssignTeam(int32 in_team);

	int32 GetTeam();

	bool SelectBoxOn()
	{
		return select_box_on;
	}

	UFUNCTION(Reliable, Client)
	void SetTargetedAbility(UObject* in_ability, int32 in_ability_num);

	void GetSelectBox(FVector2D& out_box_start, FVector2D& out_box_end)
	{
		out_box_start = select_box_start;
		out_box_end = select_box_end;
	}

	UFUNCTION(reliable, Server)
	void SetSelectedServer(const TArray<AAghsCloneCharacter*>& in_selected);

	void SetSelected(const TArray<AAghsCloneCharacter*>& in_selected);

	TArray<AAghsCloneCharacter*>& GetSelected()
	{
		return selected_units;
	}

	bool IsUnitControllable(AAghsCloneCharacter* in_unit);

	UFUNCTION(Reliable, Client)
	void SetLocalActorVisibility(AActor* in_actor, bool is_visible);

	UFUNCTION(Reliable, Server)
	void RequestBuy(int32 item_id);

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;
	bool shop_on = false;
	bool hud_clicked = false;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	UFUNCTION(reliable, server)
	void SetNewMoveDestination(const FVector DestLocation);

	UFUNCTION(reliable, server)
	void CommandAttack(const FCommand& in_command);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	UFUNCTION(reliable, server)
    void ActivateAbility(int32 ability_num);

	void OnAbilityNumPress(int32 ability_num);

	template<int32 ability_num>
	void OnAbilityPress();

	template<int32 ability_num>
	void OnAbilityRelease();

	void OnShopPress();

	void OnLeftClick();

	void OnAbilityTrigger();

	UFUNCTION(reliable, server)
	void OnTrigger(FHitResult Hit, int32 ability_num);

	void OnTriggerRelease();

	void CreateStoreWidget();

	void CleanSelected()
	{
		return;
		// Check if any of our selected units have been destroyed
		temp_units = selected_units;
		for (auto& s : temp_units)
		{
			if (!IsValid(s))
			{
				s = nullptr;
			}
		}
		temp_units.Remove(nullptr);
		SetSelected(temp_units);
	}


};



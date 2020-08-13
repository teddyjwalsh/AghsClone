// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "Ability.h"
#include "AghsClonePlayerController.generated.h"

UCLASS()
class AAghsClonePlayerController : public APlayerController
{
	GENERATED_BODY()

	float mx, my;
	bool select_box_on;
	
	FVector2D select_box_start;
	FVector2D select_box_end;
	UAbility* targeted_ability;
	int32 targeted_ability_num;

	TArray<AAghsCloneCharacter*> selected_units;

public:
	int32 team;

	AAghsClonePlayerController();

	UFUNCTION(Reliable, Client)
	void AssignTeam(int32 in_team);

	int32 GetTeam();

	bool SelectBoxOn()
	{
		return select_box_on;
	}

	void GetSelectBox(FVector2D& out_box_start, FVector2D& out_box_end)
	{
		out_box_start = select_box_start;
		out_box_end = select_box_end;
	}

	UFUNCTION(reliable, server)
	void SetSelected(const TArray<AAghsCloneCharacter*>& in_selected);

	void GetSelected(TArray<AAghsCloneCharacter*>& out_selected)
	{
		out_selected = selected_units;
	}

	UFUNCTION(Reliable, Client)
	void SetLocalActorVisibility(AActor* in_actor, bool is_visible);


protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
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

	void OnLeftClick();

	UFUNCTION(reliable, server)
	void OnTrigger();

	void OnTriggerRelease();
};



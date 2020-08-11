// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFramework/PlayerController.h"
#include "AghsClonePlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/SpringArmComponent.h"

AAghsClonePlayerController::AAghsClonePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	
	bReplicates = true;
	//SetReplicateMovement(true);
}

void AAghsClonePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	int32 wx, wy;

	GetMousePosition(mx, my);
	GetViewportSize(wx, wy);

	select_box_end = FVector2D(mx, my);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
		bMoveToMouseCursor = false;
	}

	AUnitController* MyPawn = Cast<AUnitController>(GetPawn());
	if (MyPawn != nullptr)
	{
		auto cb = MyPawn->GetCameraBoom();
		if (abs(mx - wx) <= 20)
		{
			cb->AddWorldOffset(FVector(0, 10, 0));
		}
		if (abs(my - wy) <= 20)
		{
			cb->AddWorldOffset(FVector(-10, 0, 0));
		}
		if (abs(mx - 0) <= 20)
		{
			cb->AddWorldOffset(FVector(0, -10, 0));
		}
		if (abs(my - 0) <= 20)
		{
			cb->AddWorldOffset(FVector(10, 0, 0));
		}
	}
}

void AAghsClonePlayerController::SetSelected_Implementation(const TArray<AAghsCloneCharacter*>& in_selected)
{
	AUnitController* MyPawn = Cast<AUnitController>(GetPawn());
	if (MyPawn != nullptr)
	{
		MyPawn->SetSelected(in_selected);
		selected_units = in_selected;
	}
}

void AAghsClonePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AAghsClonePlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("Trigger", IE_Pressed, this, &AAghsClonePlayerController::OnLeftClick);
	InputComponent->BindAction("Trigger", IE_Released, this, &AAghsClonePlayerController::OnTriggerRelease);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AAghsClonePlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("Ability1", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<0>);
	InputComponent->BindAction("Ability2", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<1>);
	InputComponent->BindAction("Ability3", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<2>);
	InputComponent->BindAction("Ability4", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<3>);
	InputComponent->BindAction("Ability1", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<0>);
	InputComponent->BindAction("Ability2", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<1>);
	InputComponent->BindAction("Ability3", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<2>);
	InputComponent->BindAction("Ability4", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<3>);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AAghsClonePlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AAghsClonePlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AAghsClonePlayerController::OnResetVR);

	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(false);
	SetInputMode(InputMode);
}

void AAghsClonePlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AAghsClonePlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AAghsCloneCharacter* MyPawn = Cast<AAghsCloneCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void AAghsClonePlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AAghsClonePlayerController::SetNewMoveDestination_Implementation(const FVector DestLocation)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 10.0))
		{
			//MyPawn->SetDestination(DestLocation);
			FCommand move_command;
			move_command.command_type = MOVE;
			move_command.location = DestLocation;
			MyPawn->SetCommand(move_command);
		}
	}
}

void AAghsClonePlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;

	UE_LOG(LogTemp, Warning, TEXT("RIGHT CLOCK"));
}

void AAghsClonePlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;

}

template<int32 ability_num>
void AAghsClonePlayerController::OnAbilityPress()
{
	OnAbilityNumPress(ability_num);
}

void AAghsClonePlayerController::OnAbilityNumPress_Implementation(int32 ability_num)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		AAghsCloneCharacter * main_character = MyPawn->GetPrimaryUnit();
		if (main_character)
		{
			if (main_character->Abilities.size() > ability_num)
			{
				if (main_character->Abilities[ability_num]->bUnitTargeted || main_character->Abilities[ability_num]->bGroundTargeted)
				{
					// main_character->SetTargetingActive(ability_num);
					targeted_ability = main_character->Abilities[ability_num];
					targeted_ability_num = ability_num;
				}
				else if (main_character->Abilities[ability_num]->bToggleable && GetLocalRole() == ROLE_Authority)
				{
					main_character->Abilities[ability_num]->bToggled = !main_character->Abilities[ability_num]->bToggled;
				}
				else if (!main_character->Abilities[ability_num]->bPassive && GetLocalRole() == ROLE_Authority)
				{
					main_character->Abilities[ability_num]->OnActivationMeta();
				}
			}
		}
	}
	if (AAghsCloneCharacter* MyPawn = Cast<AAghsCloneCharacter>(GetPawn()))
	{
		if (MyPawn->Abilities.size() > ability_num)
		{
			if (MyPawn->Abilities[ability_num]->bUnitTargeted || MyPawn->Abilities[ability_num]->bGroundTargeted)
			{
				MyPawn->SetTargetingActive(ability_num);
			}
			else if (MyPawn->Abilities[ability_num]->bToggleable)
			{
				MyPawn->Abilities[ability_num]->bToggled = !MyPawn->Abilities[ability_num]->bToggled;
			}
			else if (!MyPawn->Abilities[ability_num]->bPassive)
			{
				MyPawn->Abilities[ability_num]->OnActivationMeta();
			}
		}
	}
}

template<int32 ability_num>
void AAghsClonePlayerController::OnAbilityRelease()
{
	
}

void AAghsClonePlayerController::OnLeftClick()
{
	if (!targeted_ability)
	{
		select_box_on = true;
		select_box_start = FVector2D(mx, my);
	}
	else
	{
		OnTrigger();
	}
}

void AAghsClonePlayerController::OnTrigger_Implementation()
{
	{
		UE_LOG(LogTemp, Warning, TEXT("LEFT CLOCK"));
		if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
		{
			// Trace to see what is under the mouse cursor
			FHitResult Hit;
			GetHitResultUnderCursor(ECC_Visibility, false, Hit);
			AAghsCloneCharacter* main_character = MyPawn->GetPrimaryUnit();
			if (Hit.bBlockingHit && main_character)
			{
				//MyPawn->TriggerTargetedAbility(Hit.ImpactPoint);
				FCommand move_command;
				move_command.command_type = ABILITY;
				move_command.ability_num = targeted_ability_num;// main_character->GetTargetingActive();
				move_command.location = Hit.ImpactPoint;
				main_character->SetCommand(move_command);
				
			}
			targeted_ability = nullptr;
		}
	}
}

void AAghsClonePlayerController::OnTriggerRelease()
{
	select_box_on = false;
}
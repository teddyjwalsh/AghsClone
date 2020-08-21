// Copyright Epic Games, Inc. All Rights Reserved.

#include "AghsClonePlayerController.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PointLightComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "AghsCloneCharacter.h"
#include "UnitController.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "GameFramework/SpringArmComponent.h"
#include "StoreWidget.h"
#include "FieldActorInterface.h"
#include "WalletComponent.h"
#include "InventoryComponent.h"
#include "Shop.h"

AAghsClonePlayerController::AAghsClonePlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	ClickEventKeys.Add(EKeys::RightMouseButton);
	//bReplicates = true;
	//SetReplicateMovement(true);
}

void AAghsClonePlayerController::BeginPlay()
{
	shop_on = false;
}

void AAghsClonePlayerController::AssignTeam_Implementation(int32 in_team)
{
	team = in_team;
}

int32 AAghsClonePlayerController::GetTeam()
{
	return team;
}

void AAghsClonePlayerController::SetTargetedAbility_Implementation(UObject* in_ability, int32 in_ability_num)
{
	auto ability = Cast<IAbilityInterface>(in_ability);
	targeted_ability = ability;
	targeted_ability_num = in_ability_num;
}

void AAghsClonePlayerController::SetLocalActorVisibility_Implementation(AActor* in_actor, bool is_visible)
{
	//in_actor->SetActorHiddenInGame(!is_visible);
	auto field_actor = Cast<IFieldActorInterface>(in_actor);
	if (field_actor)
	{
		auto ap = Cast<AAghsCloneCharacter>(in_actor);
		//ap->GetMesh()->SetVisibility(is_visible);
		//ap->GetCapsuleComponent()->SetVisibleFlag(is_visible);
		in_actor->SetActorHiddenInGame(!is_visible);
		if (ap)
		{
			if (ap->GetTeam() != GetTeam())
			{
				ap->GetVisionLight()->SetVisibility(false);
			}
		}
		else
		{

		}
	}
}

void AAghsClonePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	GetMousePosition(mx, my);
	GetViewportSize(wx, wy);

	select_box_end = FVector2D(mx, my);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
		bMoveToMouseCursor = false;
	}

	if (IsValid(StoreWidget))
	{
		if (StoreWidget->GridIsHovered())
		{
			hud_clicked = true;
		}
		else
		{
			hud_clicked = false;
		}
	}
	if (IsValid(AbilitiesWidget))
	{
		AbilitiesWidget->RefreshCooldownDisplays();
	}
	if (IsValid(InventoryWidget))
	{
		InventoryWidget->SetItems();
		InventoryWidget->RefreshCooldownDisplays();
	}

	AUnitController* MyPawn = Cast<AUnitController>(GetPawn());
	if (MyPawn != nullptr)
	{
		if (InventoryWidget)
		{
			//InventoryWidget->SetItems();
		}
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

void AAghsClonePlayerController::SetSelectedServer_Implementation(const TArray<AAghsCloneCharacter*>& in_selected)
{
	AUnitController* MyPawn = Cast<AUnitController>(GetPawn());
	if (MyPawn != nullptr)
	{
		MyPawn->SetSelected(in_selected);
		selected_units = in_selected;
	}
}

void AAghsClonePlayerController::SetSelected(const TArray<AAghsCloneCharacter*>& in_selected)
{
	AUnitController* MyPawn = Cast<AUnitController>(GetPawn());
	if (MyPawn != nullptr)
	{
		selected_units.Reserve(in_selected.Num());
		for (auto& u : in_selected)
		{
			if (IsUnitControllable(u))
			{
				selected_units.Add(u);
			}
		}
		MyPawn->SetSelected(selected_units);
		MyPawn->selected = selected_units;
		if (!GetWorld()->IsServer())
		{
			SetSelectedServer(selected_units);
		}

		if (IsLocalController() && selected_units.Num())
		{
			if (!InventoryWidget)
			{
				InventoryWidgetClass = UInventoryWidget::StaticClass();
				InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
				InventoryWidget->SetPositionInViewport(FVector2D(wx - 120, wy - 80));
				InventoryWidget->AddToViewport(9999); // Z-order, this just makes it render on the very top.
				InventoryWidget->SetItems();
			}
			else
			{
				InventoryWidget->DrawInventory();
				InventoryWidget->SetItems();
				InventoryWidget->AddToViewport(9999); // Z-order, this just makes it render on the very top.
				InventoryWidget->SetPositionInViewport(FVector2D(wx - 120, wy - 80));
				FAnchors anchor(100, 100, 100, 100);
				InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			}
			if (!AbilitiesWidget)
			{
				AbilitiesWidgetClass = UAbilitiesWidget::StaticClass();
				AbilitiesWidget = CreateWidget<UAbilitiesWidget>(this, AbilitiesWidgetClass);
				AbilitiesWidget->SetAlignmentInViewport(FVector2D(0.0, 1.0));
				AbilitiesWidget->SetPositionInViewport(FVector2D(0, wy), true);
				AbilitiesWidget->AddToViewport(9999); // Z-order, this just makes it render on the very top.
				AbilitiesWidget->SetAbilities();
				AbilitiesWidget->DrawAbilities();
			}
			else
			{
				//AbilitiesWidget->SetAnchorsInViewport(FAnchors(0.5, 0.5));
				AbilitiesWidget->SetAlignmentInViewport(FVector2D(0.0, 1.0));
				AbilitiesWidget->SetPositionInViewport(FVector2D(0, wy), true);
				AbilitiesWidget->DrawAbilities();
				AbilitiesWidget->SetAbilities();
				AbilitiesWidget->AddToViewport(9999); // Z-order, this just makes it render on the very top.
				
				FAnchors anchor(100, 100, 100, 100);
				AbilitiesWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}

bool AAghsClonePlayerController::IsUnitControllable(AAghsCloneCharacter* in_unit)
{
	if (in_unit->GetTeam() == team)
	{
		if (in_unit->GetUnitOwner() == GetPawn())
		{
			return true;
		}
	}
	return false;
}

void AAghsClonePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AAghsClonePlayerController::OnSetDestinationPressed); void OnShopPress();
	InputComponent->BindAction("Trigger", IE_Pressed, this, &AAghsClonePlayerController::OnLeftClick);
	InputComponent->BindAction("Trigger", IE_Released, this, &AAghsClonePlayerController::OnTriggerRelease);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AAghsClonePlayerController::OnSetDestinationReleased);
	InputComponent->BindAction("StopCommand", IE_Pressed, this, &AAghsClonePlayerController::OnStopCommand);
	InputComponent->BindAction("Ability1", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<0>);
	InputComponent->BindAction("Ability2", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<1>);
	InputComponent->BindAction("Ability3", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<2>);
	InputComponent->BindAction("Ability4", IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<3>);
	InputComponent->BindAction("Ability1", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<0>);
	InputComponent->BindAction("Ability2", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<1>);
	InputComponent->BindAction("Ability3", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<2>);
	InputComponent->BindAction("Ability4", IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<3>);
	InputComponent->BindAction("Item1",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<4>);
	InputComponent->BindAction("Item2",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<5>);
	InputComponent->BindAction("Item3",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<6>);
	InputComponent->BindAction("Item4",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<7>);
	InputComponent->BindAction("Item5",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<8>);
	InputComponent->BindAction("Item6",IE_Pressed, this, &AAghsClonePlayerController::OnAbilityPress<9>);
	InputComponent->BindAction("Item1",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<4>);
	InputComponent->BindAction("Item2",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<5>);
	InputComponent->BindAction("Item3",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<6>);
	InputComponent->BindAction("Item4",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<7>);
	InputComponent->BindAction("Item5",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<8>);
	InputComponent->BindAction("Item6",IE_Released, this, &AAghsClonePlayerController::OnAbilityRelease<9>);
	InputComponent->BindAction("ShopOpen", IE_Pressed, this, &AAghsClonePlayerController::OnShopPress);

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
		GetHitResultUnderCursor(ECC_WorldDynamic, false, Hit);

		auto field_unit = Cast<IFieldActorInterface>(Hit.Actor);
		if (field_unit)
		{
			FCommand move_command;
			move_command.command_type = ATTACK_MOVE;
			move_command.target = Hit.Actor.Get();
			CommandAttack(move_command);
		}
		else if (Hit.bBlockingHit)
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
	CleanSelected();
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

void AAghsClonePlayerController::CommandAttack_Implementation(const FCommand& in_command)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		MyPawn->SetCommand(in_command);
	}
}

void AAghsClonePlayerController::OnSetDestinationPressed()
{
	if (StoreWidget)
	{
		if (StoreWidget->GridIsHovered())
		{
			//hud_clicked = true;
		}
	}
	// set flag to keep updating destination until released
	if (!hud_clicked)
	{
		bMoveToMouseCursor = true;
	}

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

void AAghsClonePlayerController::ActivateAbility_Implementation(int32 ability_num)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		IAbilityContainerInterface* main_character = Cast<IAbilityContainerInterface>(MyPawn->GetPrimaryUnit());
		if (main_character)
		{
			if (main_character->AbilityCount() > ability_num)
			{
                IAbilityInterface* ability = main_character->GetAbility(ability_num);
				if (ability->IsToggleable() && GetLocalRole() == ROLE_Authority)
				{
					ability->Toggle();// = !ability->IsToggled();
				}
				else if (!ability->IsPassive() && GetLocalRole() == ROLE_Authority)
				{
					ability->OnActivationMeta();
				}
            }
        }
    }
}

void AAghsClonePlayerController::OnAbilityNumPress(int32 ability_num)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		IAbilityContainerInterface* main_character = Cast<IAbilityContainerInterface>(MyPawn->GetPrimaryUnit());
		if (main_character)
		{
			if (main_character->AbilityCount() > ability_num)
			{
                IAbilityInterface* ability = main_character->GetAbility(ability_num);
				if (ability)
				{
					if (ability->IsUnitTargeted() || ability->IsGroundTargeted())
					{
						// main_character->SetTargetingActive(ability_num);
						targeted_ability = ability;
						targeted_ability_num = ability_num;
					}
					else
					{
						ActivateAbility(ability_num);
					}
				}
			}
		}
	}
}

template<int32 ability_num>
void AAghsClonePlayerController::OnAbilityRelease()
{
	
}

void AAghsClonePlayerController::OnShopPress()
{
	if (IsLocalController())
	{
		if (!shop_on)
		{
			StoreWidgetClass = UStoreWidget::StaticClass();
			StoreWidget = CreateWidget<UStoreWidget>(this, StoreWidgetClass);
			if (!StoreWidget)
			{
				shop_on = false;
			}
			else
			{
				//FInputModeGameAndUI Mode;
				//Mode.SetLockMouseToViewportBehavior(EMouseLockMode::true);
				//Mode.SetHideCursorDuringCapture(false);
				//SetInputMode(Mode);
				StoreWidget->AddToViewport(9999); // Z-order, this just makes it render on the very top.
				//StoreWidget->SetAlignmentInViewport(FVector2D(0.1, 0.1));
				//StoreWidget->SetPadding(FMargin(0.1,0.1,0,0));
				//auto canvas_slot = Cast<UCanvasPanelSlot>(StoreWidget);
				//canvas_slot->SetDesiredPosition(FVector2D(30, 30));
				StoreWidget->SetPositionInViewport(FVector2D(40, 40));
				//StoreWidget->SetDesiredSizeInViewport(FVector2D(400, 800));
				FAnchors anchor(100, 100, 100, 100);
				StoreWidget->SetVisibility(ESlateVisibility::Visible);
				//StoreWidget->SetAnchorsInViewport(anchor);
				shop_on = true;
			}
		}
		else
		{
			StoreWidget->RemoveFromViewport();
			shop_on = false;
		}
	}
}

void AAghsClonePlayerController::OnLeftClick()
{
	CleanSelected();

	if (!targeted_ability)
	{
		select_box_on = true;
		select_box_start = FVector2D(mx, my);
	}
	else
	{
		OnAbilityTrigger();
	}
}

void AAghsClonePlayerController::OnAbilityTrigger()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	OnTrigger(Hit, targeted_ability_num);
}

void AAghsClonePlayerController::OnTrigger_Implementation(FHitResult Hit, int32 ability_num)
{
	{
		UE_LOG(LogTemp, Warning, TEXT("LEFT CLOCK"));
		if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
		{
			
			AAghsCloneCharacter* main_character = MyPawn->GetPrimaryUnit();
			if (Hit.bBlockingHit && main_character)
			{
				FCommand ability_command;
				//MyPawn->TriggerTargetedAbility(Hit.ImpactPoint);
				ability_command.command_type = ABILITY;
				ability_command.ability_num = ability_num;// main_character->GetTargetingActive();
				auto unit = Cast<AAghsCloneCharacter>(Hit.Actor);
				if (unit && targeted_ability->IsUnitTargeted())
				{
					ability_command.target = Hit.Actor.Get();
					ability_command.unit_targeted = true;
				}
				else
				{
					ability_command.location = Hit.ImpactPoint;
				}
				main_character->SetCommand(ability_command);
			}
			SetTargetedAbility(nullptr, -1);
			targeted_ability = nullptr;
		}
	}
}

void AAghsClonePlayerController::OnTriggerRelease()
{
	select_box_on = false;
}

void AAghsClonePlayerController::RequestBuy_Implementation(int32 item_id)
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		auto wallet = Cast<UWalletComponent>(MyPawn->GetPrimaryUnit()->GetComponentByClass(UWalletComponent::StaticClass()));
		auto inventory_comp = Cast<UInventoryComponent>(MyPawn->GetPrimaryUnit()->GetComponentByClass(UInventoryComponent::StaticClass()));
		auto Shop = AShop::GetClosestShop(MyPawn, item_id);
		if (Shop)
		{
			if (wallet && inventory_comp && Shop->CanBuy(wallet->GetOwner()))
			{
				if (wallet->Debit(Shop->GetPrice(item_id)))
				{
					inventory_comp->InsertItem(Shop->BuyItem(item_id));
				}
			}
		}
	}
}

void AAghsClonePlayerController::OnStopCommand_Implementation()
{
	if (AUnitController* MyPawn = Cast<AUnitController>(GetPawn()))
	{
		FCommand stop_command;
		stop_command.command_type = STOP;
		MyPawn->SetCommand(stop_command);
	}
}
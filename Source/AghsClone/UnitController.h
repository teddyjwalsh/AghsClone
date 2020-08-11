// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CommandInterface.h"
#include "Components/DecalComponent.h"
#include "AghsCloneCharacter.h"
#include "UnitController.generated.h"

UCLASS()
class AGHSCLONE_API AUnitController : public APawn, public ICommandInterface
{
	GENERATED_BODY()

	TArray<AAghsCloneCharacter*> selected;
	AAghsCloneCharacter* my_character;
	
public:
	// Sets default values for this pawn's properties
	AUnitController();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetSelected(const TArray<AAghsCloneCharacter*>& in_selected)
	{
		selected = in_selected;
	}

	AAghsCloneCharacter* GetPrimaryUnit()
	{
		if (selected.Num())
		{
			return selected[0];
		}
		return nullptr;
	}

	virtual void QueueCommand(const FCommand& in_command) override
	{
		for (auto& unit : selected)
		{
			unit->QueueCommand(in_command);
		}
	}

	virtual void ClearCommandQueue() override
	{
		for (auto& unit : selected)
		{
			unit->ClearCommandQueue();
		}
	}

	virtual void SetCommand(const FCommand& in_command) override
	{
		for (auto& unit : selected)
		{
			unit->SetCommand(in_command);
		}
	}

	virtual void NextCommand() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Command"));
		for (auto& unit : selected)
		{
			unit->NextCommand();
		}
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <vector>
#include <deque>

#include "Containers/Queue.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"

#include "Ability.h"
#include "Components/DecalComponent.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "CommandInterface.h"
#include "AghsCloneCharacter.generated.h"

class CommonUnitStats
{
public:
	CommonUnitStats():
		MaxHealth(100.0),
		HealthRegeneration(0.1),
		Armor(0),
		MagicResist(0.25),
		IsSpellImmune(false),
		IsAttackImmune(false),
		MaxMana(100.0)
	{ 
		Health = MaxHealth; 
		Mana = MaxMana;
	}
	UPROPERTY(Replicated)
	float MaxHealth;
	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float HealthRegeneration;
	UPROPERTY(Replicated)
	float Armor;
	float MagicResist;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float MaxMana;
	bool IsSpellImmune;
	bool IsAttackImmune;
};

UCLASS(Blueprintable)
class AAghsCloneCharacter : public ACharacter, 
	public IHealthInterface,
	public IManaInterface,
	public ICommandInterface
	//public CommonUnitStats
	
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	float MaxHealth;
	UPROPERTY(Replicated)
		float Health;
	UPROPERTY(Replicated)
		float HealthRegeneration;
	UPROPERTY(Replicated)
		float Armor;
	float MagicResist;
	UPROPERTY(Replicated)
		float Mana;
	UPROPERTY(Replicated)
		float MaxMana;
	bool IsSpellImmune;
	bool IsAttackImmune;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 team;

public:
	AAghsCloneCharacter();

	// Called every frame.
	// UFUNCTION(reliable, server)
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	int32 GetTeam()
	{
		return team;
	}

	// Called when the game starts or when spawned
	virtual void BeginPlay()
	{
		Super::BeginPlay();
		PrimaryActorTick.bCanEverTick = (GetLocalRole() == ROLE_Authority);
	}

	void SetSelected(bool is_selected)
	{
		CursorToWorld->SetVisibility(is_selected);
	}

	virtual float GetHealth() const override
	{
		return Health;
	}

	virtual float GetDelayedHealth() const override
	{
		return Health;
	}

	virtual void SetHealth(float in_val) override
	{
		Health = std::min(GetMaxHealth(), std::max(0.0f, in_val));
		if (Health == 0)
		{
			Destroy();
		}
	}

	virtual void SetMaxHealth(float in_val)
	{
		MaxHealth = in_val;
	}

	virtual float GetArmor() const override
	{
		return Armor;
	}

	virtual float GetMagicResist() const override
	{
		return MagicResist;
	}

	virtual float GetMaxHealth() const override
	{
		return MaxHealth;
	}

	virtual float GetMana()
	{
		return Mana;
	}

	virtual void SetMana(float in_val)
	{
		Mana = std::min(GetMaxMana(), std::max(0.0f, in_val));
	}

	virtual void SetMaxMana(float in_val)
	{
		MaxMana = in_val;
	}

	virtual float GetMaxMana() const
	{
		return MaxMana;
	}

	void SetTargetingActive(int32 ability_num)
	{
		int count = 1;
		for (auto& ab : Abilities)
		{
			if (ability_num != count)
			{
				//ab->TargetingDecal->SetVisibility(false);
			}
			else
			{
				ab->TargetingDecal->SetVisibility(true);
			}
			count += 1;
		}
		//CursorToWorld->SetVisibility(false);
		//Abilities[ability_num]->TargetingDecal->SetVisibility(true);
		//targeting_active = ability_num;
	}

	int32 GetTargetingActive()
	{
		return targeting_active;
	}

	FVector GetDestination()
	{
		return current_destination;
	}

	void SetDestination(FVector in_destination)
	{
		current_destination = in_destination;
	}

	bool TriggerTargetedAbility(FVector target_loc)
	{
		bool retval = false;
		if (targeting_active >= 0 && targeting_active < Abilities.size())
		{
			if ((GetActorLocation() - target_loc).Size() < Abilities[targeting_active]->CastRange)
			{
				Abilities[targeting_active]->OnGroundActivationMeta(target_loc);
				retval = true;
			}
			Abilities[targeting_active]->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}
		targeting_active = -1;
		return retval;
	}

	bool TriggerTargetedAbility(int32 ability_num, FVector target_loc)
	{
		bool retval = false;
		if (ability_num >= 0 && ability_num < Abilities.size())
		{
			if ((GetActorLocation() - target_loc).Size() < Abilities[ability_num]->CastRange)
			{
				Abilities[ability_num]->OnGroundActivationMeta(target_loc);
				retval = true;
			}
			Abilities[ability_num]->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}
		return retval;
	}

	//UFUNCTION(reliable, server)
	virtual void QueueCommand(const FCommand& in_command) override
	{
		UE_LOG(LogTemp, Warning, TEXT("Command Queued, %d"), command_queue.IsEmpty());
		command_queue.Enqueue(in_command);
	}

	//UFUNCTION(reliable, server, WithValidation)
	virtual void ClearCommandQueue() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Command Queue Cleared"));
		command_queue.Empty();
	}

	//UFUNCTION(reliable, server, WithValidation)
	virtual void SetCommand(const FCommand& in_command) override
	{
		ClearCommandQueue();
		current_command = in_command;
	}

	virtual void NextCommand() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Command"));
		if (!command_queue.IsEmpty())
		{
			command_queue.Dequeue(current_command);
			destination_set = false;
		}
		else
		{
			current_command.command_type = NONE;
		}
	}

	UFUNCTION()
	void CommandStateMachine(float dt);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AAghsCloneCharacter, Mana);
	}

	void GetFOV()
	{
		int32 angle_division = 100;
		TSet<AActor*> actors_in_radius;
		VisionBounds->GetOverlappingActors(actors_in_radius);

		for (auto& act : actors_in_radius)
		{
			auto test_char = Cast<AAghsCloneCharacter>(act);
			FHitResult out_hit;
			FVector shoot_vector = act->GetActorLocation() - GetActorLocation();
			shoot_vector.Normalize();
			GetWorld()->LineTraceSingleByChannel(out_hit, GetActorLocation(), shoot_vector * VisionBounds->GetScaledSphereRadius(), ECollisionChannel::ECC_WorldDynamic);
			auto seen_char = Cast<AAghsCloneCharacter>(out_hit.Actor);
			if (seen_char)
			{
				if (out_hit.Actor->GetUniqueID() != act->GetUniqueID() && test_char->GetTeam() != 1)
				{
					act->SetActorHiddenInGame(true);
				}
				else
				{
					act->SetActorHiddenInGame(false);
				}
			}
			else
			{
				act->SetActorHiddenInGame(true);
			}
		}
		/*
		for (int i = 0; i < angle_division; ++i)
		{
			float angle = i * 360.0 / angle_division;
			float shoot_distance = 1000;
			FHitResult out_hit;
			FVector shoot_vector(cos(angle), sin(angle), 0);
			shoot_vector.Normalize();
			
		}
		*/
	}

	std::vector<UAbility*> Abilities;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* VisionBounds;

	

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	int32 targeting_active;

	TQueue<FCommand> command_queue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCommand current_command;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	FVector current_destination;
	bool destination_set;
};


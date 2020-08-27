// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Actor.h"
#include "CommandInterface.generated.h"

UENUM(BlueprintType)
enum CommandType
{
	NONE UMETA(DisplayName = "NONE"),
	MOVE UMETA(DisplayName = "MOVE"),
	ATTACK_MOVE UMETA(DisplayName = "ATTACK_MOVE"),
	ABILITY UMETA(DisplayName = "ABILITY"),
	STOP UMETA(DisplayName = "STOP")
};

USTRUCT(BlueprintType)
struct FCommand
{
public:
	GENERATED_USTRUCT_BODY()

	FCommand() : command_type(NONE) {}
	FCommand(CommandType in_type): command_type(in_type){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CommandType)
	TEnumAsByte<CommandType> command_type;
	UPROPERTY(BlueprintReadWrite)
	int ability_num;
	UPROPERTY(BlueprintReadWrite)
	FVector location;
	UPROPERTY(BlueprintReadWrite)
	AActor* target;
	UPROPERTY(BlueprintReadWrite)
	int32 item_id;
	bool unit_targeted = false;

	inline bool operator!=(const FCommand& rhs) const
	{
		if (command_type != rhs.command_type)
		{
			return true;
		}
		if (command_type == MOVE)
		{
			if (location != rhs.location)
			{
				return true;
			}
			if (target != rhs.target)
			{
				return true;
			}
		}
		else if (command_type == ATTACK_MOVE)
		{
			if (location != rhs.location)
			{
				return true;
			}
			if (target != rhs.target)
			{
				return true;
			}
		}
        else if (command_type == ABILITY)
        {
            if (ability_num != rhs.ability_num)
            {
                return true;
            }
            else if (unit_targeted != rhs.unit_targeted)
            {
                return true;
            }
            else if (unit_targeted && (target != rhs.target))
            {
                return true;
            }
            else if (!unit_targeted && (location != rhs.location))
            {
                return true;
            }
        }
		return false;
	}
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCommandInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AGHSCLONE_API ICommandInterface
{
	GENERATED_BODY()

public:
	virtual void QueueCommand(const FCommand& in_command) = 0;

	virtual void ClearCommandQueue() = 0;

	virtual void SetCommand(const FCommand& in_command) = 0;

	virtual void NextCommand() = 0;

	virtual FCommand GetCurrentCommand() const
	{
		UE_LOG(LogTemp, Fatal, TEXT("Called GetCurrentCommand without implementing it"));
		return FCommand(NONE);
	}

	virtual FVector GetCurrentDestination() const
	{
		UE_LOG(LogTemp, Fatal, TEXT("Called GetCurrentDestination without implementing it"));
		return FVector(0);
	}

	virtual void SetCurrentDestination(FVector in_dest)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Called SetCurrentDestination without implementing it"));
	}

	virtual FCommand GetLastCommand() const
	{
		return FCommand();
	}
	virtual void SetLastCommand(const FCommand& in_command)
	{

	}

	virtual void ProcessAbilityCommand(const FCommand& in_command, float dt, bool is_new) {}

	virtual void ProcessAttackMove(const FCommand& in_command, float dt, bool is_new) {}

	virtual void CommandStateMachine(float dt)
	{
		static float MoveTolerance = 10.0;
		FCommand current_command = GetCurrentCommand();
		FCommand last_command = GetLastCommand();
		FVector dest = GetCurrentDestination();
		auto my_actor = Cast<APawn>(this);
        bool is_new = false;

		if (current_command.command_type != last_command.command_type)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(my_actor->GetController(), my_actor->GetActorLocation());
            is_new = true; 
		}
		switch (current_command.command_type)
		{
			case MOVE:
			{
				if (my_actor)
				{
					auto my_loc3 = my_actor->GetActorLocation();
					auto my_loc = FVector2D(my_loc3);
					if ((my_loc - FVector2D(current_command.location)).Size() < MoveTolerance)
					{
						UE_LOG(LogTemp, Warning, TEXT("Completed Move"));
						NextCommand();
					}
					else if (GetCurrentDestination() != current_command.location)
					{
						SetCurrentDestination(current_command.location);
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(my_actor->GetController(), GetCurrentDestination());
					}
					break;
				}
				break;
			}
			case ATTACK_MOVE:
			{
				ProcessAttackMove(current_command, dt, is_new);
				break;
			}
			case ABILITY:
			{
				ProcessAbilityCommand(current_command, dt, is_new);
				break;
			}
			case STOP:
			{
				ClearCommandQueue();
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(my_actor->GetController(), my_actor->GetActorLocation());
			}
			default:
				break;
		}
		SetLastCommand(current_command);
	}

};

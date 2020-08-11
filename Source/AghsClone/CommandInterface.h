// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CommandInterface.generated.h"

UENUM(BlueprintType)
enum CommandType
{
	NONE UMETA(DisplayName = "NONE"),
	MOVE UMETA(DisplayName = "MOVE"),
	ATTACK_MOVE UMETA(DisplayName = "ATTACK_MOVE"),
	ABILITY UMETA(DisplayName = "ABILITY")
};

USTRUCT(BlueprintType)
struct FCommand
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CommandType)
	TEnumAsByte<CommandType> command_type;
	UPROPERTY(BlueprintReadWrite)
	int ability_num;
	UPROPERTY(BlueprintReadWrite)
	FVector location;

	inline bool operator!=(const FCommand& rhs) const
	{
		if (command_type != rhs.command_type)
		{
			return false;
		}
		if (command_type == MOVE)
		{
			if (location != rhs.location)
			{
				return false;
			}
		}
		return true;
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

};

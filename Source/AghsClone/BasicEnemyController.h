// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "EnemyCharacter.h"

#include "BasicEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class AGHSCLONE_API ABasicEnemyController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* vision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* sight;

	enum EnemyState
	{
		Idle,
		Attack,
		Patrol,
	};

	EnemyState my_state = Idle;
	AActor* to_attack = nullptr;
	bool attack_in_sight = false;
	float max_visionless_time = 2;
	float visionless_time = 0;

	ABasicEnemyController()
	{
		vision = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Vision"));
		sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
		vision->OnPerceptionUpdated.AddDynamic(this, &ABasicEnemyController::OnSightUpdate);

		sight->DetectionByAffiliation.bDetectNeutrals = true;
		sight->DetectionByAffiliation.bDetectEnemies = true;
		sight->PeripheralVisionAngleDegrees = 90;
		vision->ConfigureSense(*sight);
	}

	UFUNCTION()
	void OnSightUpdate(const TArray<AActor*>& UpdatedActors)
	{
		AEnemyCharacter* my_char = Cast<AEnemyCharacter>(GetPawn());
		TArray<AActor*> out_actors;
		vision->GetPerceivedActors(UAISense_Sight::StaticClass(), out_actors);
		if (my_char)
		{
			AActor* closest = to_attack;
			if (to_attack)
			{
				if (out_actors.Find(to_attack) != INDEX_NONE)
				{
					closest = to_attack;
				}
				else
				{
					closest = to_attack;
					attack_in_sight = false;
				}
			}
			for (auto& ua : out_actors)
			{
				AAghsCloneCharacter* other_char = Cast<AAghsCloneCharacter>(ua);
				if (other_char && !to_attack && other_char->GetTeam() != my_char->GetTeam())
				{
					closest = ua;
				}
			}
			if (closest)
			{
				SetState(Attack, closest);
			}
			else
			{
				SetState(Idle, nullptr);
			}
		}
	}

	void SetState(EnemyState in_state, AActor* target)
	{
		AEnemyCharacter* my_char = Cast<AEnemyCharacter>(GetPawn());
		if (in_state == Attack)
		{
			FCommand new_command;
			new_command.command_type = ATTACK_MOVE;
			new_command.target = target;
			new_command.unit_targeted = true;
			my_char->SetCommand(new_command);
			to_attack = target;
		}
		else if (in_state == Idle)
		{
			FCommand new_command;
			new_command.command_type = STOP;
			my_char->SetCommand(new_command);
			to_attack = nullptr;
		}
	}

	void Tick(float DeltaTime) override
	{
		if (to_attack)
		{
			//5		vision->GetPerceivedActors();
			if (vision->HasActiveStimulus(*to_attack, sight->GetSenseID()))
			{

			}
			else
			{
				visionless_time += DeltaTime;
				if ((visionless_time > max_visionless_time))
				{
					SetState(Idle, nullptr);
					visionless_time = 0;
				}
			}
		}
		
	}
};

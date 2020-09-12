// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

#include "EnemyCharacter.h"
#include "VisionManager.h"

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
	AVisionManager* vision_manager;

	ABasicEnemyController()
	{
		vision = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Vision"));
		sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight"));
		//vision->OnPerceptionUpdated.AddDynamic(this, &ABasicEnemyController::OnSightUpdate);
		sight->DetectionByAffiliation.bDetectNeutrals = true;
		sight->DetectionByAffiliation.bDetectEnemies = true;
		sight->SightRadius = 2000;
		sight->LoseSightRadius = 2200;
		sight->PeripheralVisionAngleDegrees = 90;
		vision->ConfigureSense(*sight);
		vision->SetDominantSense(UAISense_Sight::StaticClass());
		PrimaryActorTick.TickInterval = 0.2;

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVisionManager::StaticClass(), FoundActors);
		if (FoundActors.Num())
		{
			vision_manager = Cast<AVisionManager>(FoundActors[0]);
		}
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
			my_state = Attack;
		}
		else if (in_state == Idle)
		{
			FCommand new_command;
			new_command.command_type = STOP;
			my_char->SetCommand(new_command);
			to_attack = nullptr;
			my_state = Idle;
		}
	}

	void Tick(float DeltaTime) override
	{
		AEnemyCharacter* my_char = Cast<AEnemyCharacter>(GetPawn());
		TArray<AActor*> out_actors;
		vision->GetPerceivedActors(UAISense_Sight::StaticClass(), out_actors);
		std::set<AActor*> visible_actors = vision_manager->get_team_visible(my_char->GetTeam());

		switch (my_state)
		{
		case Idle:
		{
			AActor* closest = nullptr;
			for (auto& act : visible_actors)
			{
				AAghsCloneCharacter* other_char = Cast<AAghsCloneCharacter>(act);
				if (other_char)
				{
					if (other_char->GetTeam() != my_char->GetTeam())
					{
						if ((other_char->GetActorLocation() - my_char->GetActorLocation()).Size() < 1000)
						{
							if (closest)
							{
								if ((other_char->GetActorLocation() - my_char->GetActorLocation()).Size() <
									(closest->GetActorLocation() - my_char->GetActorLocation()).Size())
								{
									closest = other_char;
								}
							}
							else
							{
								closest = other_char;
							}
						}
					}
				}
			}
			if (closest)
			{
				SetState(Attack, closest);
			}
			break;
		}
		case Attack:
		{
			if (to_attack)
			{
				if (visible_actors.find(to_attack) == visible_actors.end() ||
				//if (out_actors.Find(to_attack) == INDEX_NONE ||
					(to_attack->GetActorLocation() - my_char->GetActorLocation()).Size() < 1000)
				{
					visionless_time += DeltaTime;
					if (visionless_time >= max_visionless_time)
					{
						SetState(Idle, nullptr);
						visionless_time = 0;
					}
				}
			}
			else
			{
				SetState(Idle, nullptr);
			}
			break;
		}
		case Patrol:
		{
			break;
		}
		default:
			break;
		}
	}
};

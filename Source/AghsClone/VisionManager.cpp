// Fill out your copyright notice in the Description page of Project Settings.

#include "VisionManager.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NetDriver.h"
#include "Engine/NetworkObjectList.h"


#include "VisionInterface.h"
#include "AghsClonePlayerController.h"

// Sets default values
AVisionManager::AVisionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	vision_bounds = CreateDefaultSubobject<USphereComponent>(TEXT("vision_bounds"));
	vision_bounds->SetAbsolute(true, true, true);

	vision_positions = CreateDefaultSubobject<UMaterialParameterCollection>(TEXT("vision_positions"));

	SetReplicates(false);
	
}

// Called when the game starts or when spawned
void AVisionManager::BeginPlay()
{
	Super::BeginPlay();
	first_iteration = true;
}

// Called every frame
void AVisionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GetWorld()->IsServer())
	{
		return;
	}
	TArray<AActor*> vision_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UVisionInterface::StaticClass(), vision_actors);
	TArray<AActor*> field_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UFieldActorInterface::StaticClass(), field_actors);
	auto old_sets = team_vision_sets;
	if (first_iteration)
	{
		for (auto& va : vision_actors)
		{
			auto vac = Cast<AAghsCloneCharacter>(va);
			if (vac)
			{
				old_sets[vac->GetTeam()] = std::set<AActor*>();
			}
		}
		for (auto& va : field_actors)
		{
			for (auto& team : old_sets)
			{
				team.second.insert(va);
			}
		}
		first_iteration = false;
	}
	for (auto& k : team_vision_sets)
	{
		team_vision_sets[k.first].clear();
	}
	
	for (auto& act : vision_actors)
	{
		auto aghs_unit = Cast<AAghsCloneCharacter>(act);
		int32 vision_team = aghs_unit->GetTeam();
		IVisionInterface* vision_interface = Cast<IVisionInterface>(act);
		vision_bounds->SetWorldLocation(act->GetActorLocation());
		vision_bounds->SetSphereRadius(vision_interface->GetVisionRadius());
		TSet<AActor*> near_chars;
		vision_bounds->GetOverlappingActors(near_chars);//, AAghsCloneCharacter::StaticClass());
		FCollisionQueryParams ignore_me;
		//ignore_me.AddIgnoredActor(act);
		ignore_me.AddIgnoredActors(field_actors);

		for (auto& near_actor : near_chars)
		{
			auto near_aghs_unit = Cast<AAghsCloneCharacter>(near_actor);
            if (near_aghs_unit)
            {
                if (act == near_actor || near_aghs_unit->GetTeam() == vision_team)
                {
                    team_vision_sets[vision_team].insert(near_actor);
                    continue;
                }
            }
			FHitResult out_hit;
			FVector distance_vector = near_actor->GetActorLocation() - act->GetActorLocation();
			FVector shoot_vector = distance_vector;
			shoot_vector.Normalize();

			GetWorld()->LineTraceSingleByChannel(out_hit, 
				vision_bounds->GetComponentLocation(), 
				vision_bounds->GetComponentLocation() + shoot_vector * vision_bounds->GetScaledSphereRadius(), 
				ECollisionChannel::ECC_WorldDynamic,
				ignore_me);
			float actor_distance = (act->GetActorLocation() - near_actor->GetActorLocation()).Size();
			if (out_hit.Distance > actor_distance || !out_hit.bBlockingHit)
			{
				team_vision_sets[vision_team].insert(near_actor);
				//act->Relev
			}
			else
			{
				//team_vision_sets[vision_team].erase(near_actor);
			}
		}
	}
	auto pit = GetWorld()->GetPlayerControllerIterator();
	int count = 0;
	bool new_controller = false;
	for (pit; pit; ++pit)
	{
		auto pc = Cast<AAghsClonePlayerController>(pit->Get());
		if (player_controllers.find(pc) == player_controllers.end())
		{
			new_controller = true;
		}
		if (pc)
		{
			for (auto& act : field_actors)
			{
				bool new_actor = false;
				if (field_actor_record.Find(act) == INDEX_NONE)
				{
					new_actor = true;
				}
				//if (GetGameTimeSinceCreation() > 2.0)
				{
					if (team_vision_sets[pc->GetTeam()].find(act) == team_vision_sets[pc->GetTeam()].end() &&
						(old_sets[pc->GetTeam()].find(act) != old_sets[pc->GetTeam()].end() || new_controller))
					{
						pc->SetLocalActorVisibility(act, false);
					}
					else if (team_vision_sets[pc->GetTeam()].find(act) != team_vision_sets[pc->GetTeam()].end() &&
						(old_sets[pc->GetTeam()].find(act) == old_sets[pc->GetTeam()].end() || new_controller))
					{
						pc->SetLocalActorVisibility(act, true);
					}
				}
			}
		}
		player_controllers.insert(pc);
	}
	field_actor_record = field_actors;
	return;
}

std::set<AActor*>& AVisionManager::get_team_visible(int32 in_team)
{
	return team_vision_sets[in_team];
}
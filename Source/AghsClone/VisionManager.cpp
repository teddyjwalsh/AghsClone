// Fill out your copyright notice in the Description page of Project Settings.

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NetDriver.h"
#include "Engine/NetworkObjectList.h"

#include "VisionInterface.h"
#include "VisionManager.h"
#include "AghsClonePlayerController.h"

// Sets default values
AVisionManager::AVisionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	vision_bounds = CreateDefaultSubobject<USphereComponent>(TEXT("vision_bounds"));
	vision_bounds->SetAbsolute(true, true, true);
	SetReplicates(false);
}

// Called when the game starts or when spawned
void AVisionManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> vision_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UVisionInterface::StaticClass(), vision_actors);
	if (!GetWorld()->IsServer())
	{
		return;
	}
	for (auto& k : team_vision_sets)
	{
		team_vision_sets[k.first].clear();
	}
	auto old_sets = team_vision_sets;
	for (auto& act : vision_actors)
	{
		auto aghs_unit = Cast<AAghsCloneCharacter>(act);
		int32 vision_team = aghs_unit->GetTeam();
		IVisionInterface* vision_interface = Cast<IVisionInterface>(act);
		vision_bounds->SetWorldLocation(act->GetActorLocation());
		vision_bounds->SetSphereRadius(vision_interface->GetVisionRadius());
		TSet<AActor*> near_chars;
		vision_bounds->GetOverlappingActors(near_chars, AAghsCloneCharacter::StaticClass());
		FCollisionQueryParams ignore_me;
		ignore_me.AddIgnoredActor(act);
	
		for (auto& near_actor : near_chars)
		{
			auto near_aghs_unit = Cast<AAghsCloneCharacter>(near_actor);
			if (act == near_actor || near_aghs_unit->GetTeam() == vision_team)
			{
				team_vision_sets[vision_team].insert(near_actor);
				continue;
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
			if (out_hit.Actor == near_actor)
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
	for (pit; pit; ++pit)
	{
		auto pc = Cast<AAghsClonePlayerController>(pit->Get());
		if (pc)
		{
			for (auto& act : vision_actors)
			{
				if (GetGameTimeSinceCreation() > 2.0)
				{
					if (team_vision_sets[pc->GetTeam()].find(act) == team_vision_sets[pc->GetTeam()].end())
					{
						pc->SetLocalActorVisibility(act, false);
					}
					else
					{
						pc->SetLocalActorVisibility(act, true);
					}
				}
			}
		}
	}
	return;
	for (auto& act : vision_actors)
	{
		auto pi = GetWorld()->GetPlayerControllerIterator();
		for (pi; pi; ++pi)
		{
			auto aghs_cont = Cast<AAghsClonePlayerController>(pi->Get());
			auto found = team_vision_sets[aghs_cont->GetTeam()].find(act);
			if (GetGameTimeSinceCreation() > 2.0)
			{
				if (found == team_vision_sets[aghs_cont->GetTeam()].end())
				{
					aghs_cont->SetLocalActorVisibility(act, true);
				}
				else
				{
					aghs_cont->SetLocalActorVisibility(act, false);
				}
			}
		}
		auto aghs_act = Cast<AAghsCloneCharacter>(act);
		for (auto& k : team_vision_sets)
		{
			{
				//auto aghs_unit = Cast<AAghsCloneCharacter>(act);
				//act->SetActorHiddenInGame(true);

				
					auto aghs_cont = Cast<AAghsClonePlayerController>(pi->Get());
					auto nc = pi->Get()->NetConnection;
					UNetDriver* nd;
					FNetworkObjectList fbol;
					if (nc)
					{
						nd = nc->GetDriver();
						fbol = nd->GetNetworkObjectList();
					}
					auto found = team_vision_sets[k.first].find(act);
					auto found_old = old_sets[k.first].find(act);
					if (aghs_cont->GetTeam() == aghs_act->GetTeam() || (found != team_vision_sets[k.first].end() && found_old == old_sets[k.first].end()))
					{
						//fbol.MarkActive(act, nc, nd);
						if (aghs_cont->GetPawn() && GetGameTimeSinceCreation() > 2.0)
						{
							aghs_cont->SetLocalActorVisibility(act, true);
						}
					}
					if (aghs_cont->GetTeam() != aghs_act->GetTeam() && (found == team_vision_sets[k.first].end() && found_old != old_sets[k.first].end()))
					{
						//fbol.MarkDormant(act, nc, 1, nd);
						if (aghs_cont->GetPawn() && GetGameTimeSinceCreation() > 2.0)
						{
							aghs_cont->SetLocalActorVisibility(act, false);
						}
					}
					count += 1;
			}
			//else
			{
				//act->SetActorHiddenInGame(false);
			}
		}
	}
}


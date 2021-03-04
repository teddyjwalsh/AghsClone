// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusManager.h"

void AStatusEffect::Tick(float dt)
{
	if (!HasAuthority())
	{
		return;
	}
	Super::Tick(dt);
	float now = GetWorld()->GetTimeSeconds();
	if (IsAura())
	{
		TArray<TEnumAsByte<EObjectTypeQuery>> m_objectTypes;
		//m_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
		//m_objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
		TArray<AActor*> actors_to_ignore;
		//actors_to_ignore.Add(GetOwner());
		TArray<AActor*> found_actors;
		AActor* ownr = GetOwner();
		FVector cur_loc = ownr->GetActorLocation();
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(), cur_loc,
			aura_radius, m_objectTypes, AActor::StaticClass(), actors_to_ignore,
			found_actors);
		auto applier_team = Cast<ITeamInterface>(GetApplier());
		for (auto found_actor : found_actors)
		{
			auto found_sm = Cast<UStatusManager>(
				found_actor->GetComponentByClass(UStatusManager::StaticClass()));
			auto team_interface = Cast<ITeamInterface>(found_actor);
			bool same_team = false;
			if (team_interface)
			{
				same_team = applier_team->GetTeam() == team_interface->GetTeam();
			}
			if (found_sm)
			{
				if (((same_team && bTeamOnly) || (!same_team && bEnemyOnly) || (!bTeamOnly && !bEnemyOnly)))
				{
					auto new_con = found_sm->RefreshStatus(this);
					if (new_con)
					{
						connections.insert(new_con);
					}
				}
			}
		}
	}
	for (auto sc : connections)
	{

		if (now - last_tick > tick_period)
		{
			TickConnection(sc->unit_with_status);
			last_tick = now;
		}
		
	}
	if (now - start_time > max_duration)
	{
		Destroy();
	}
};

void AStatusEffect::ApplyToStatusManager(UStatusManager* in_manager)
{
	auto new_con = in_manager->RefreshStatus(this);
	if (new_con)
	{
		connections.insert(new_con);
	}
}

// Sets default values for this component's properties
UStatusManager::UStatusManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.05f;
	// ...
}


// Called when the game starts
void UStatusManager::BeginPlay()
{
	Super::BeginPlay();
	
	// ...
	
}


// Called every frame
void UStatusManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TArray<StatusConnection*> to_remove;
	float now = GetWorld()->GetTimeSeconds();
	for (auto& status_it : statuses)
	{
		if (!IsValid(status_it->effect))
		{
			to_remove.Add(status_it);
		}
		else if (now - status_it->last_refresh > status_it->effect->linger_time && status_it->effect->IsAura())
		{
			to_remove.Add(status_it);
		}
	}
	for (auto& rem : to_remove)
	{
		statuses.Remove(rem);
		UE_LOG(LogTemp, Warning, TEXT("De-Applied status effect"));
		delete rem;
	}
	// ...
}


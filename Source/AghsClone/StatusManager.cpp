// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusManager.h"

// Sets default values for this component's properties
UStatusManager::UStatusManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
	TArray<UStatusEffect*> to_remove;
	for (auto& status_it : statuses)
	{
		status_it->TickStatus(DeltaTime);
		if (!status_it->IsAura())
		{
			if (status_it->IsFinished())
			{
				to_remove.Add(status_it);
			}
		}
		else
		{
			linger_times[status_it] -= DeltaTime;
			if (linger_times[status_it] < 0)
			{
				to_remove.Add(status_it);
				linger_times.Remove(status_it);
			}
		}
	}
	for (auto& rem : to_remove)
	{
		statuses.Remove(rem);
	}
	// ...
}


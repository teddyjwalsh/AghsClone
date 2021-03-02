// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "StatusConnection.h"
#include "Components/ActorComponent.h"
#include "StatInterface.h"
#include "StatusManager.generated.h"

UCLASS(Blueprintable)
class AGHSCLONE_API AStatusEffect : public AActor,
	public IStatInterface
{
	GENERATED_BODY()
protected:
	bool bFinished = false;
	bool bIsAura = false;
	bool bStackable = false;
	bool bTeamOnly = false;
	bool bEnemyOnly = false;
	float aura_radius;
	
	float current_duration;
	int tick_time;
	TArray<float*> stats;
	TArray<float*> mult_stats;
	AActor* owner;
	AActor* applier;
	FTimerHandle tick_timer;
	std::unordered_set<StatusConnection*> connections;
	float start_time;

public:
	float applied_time;
	float last_tick = -100.0;
	float linger_time = 0.5;
	float max_duration = 10000000000;
	bool bStunned = false;
	bool bRooted = false;
	bool bSilenced = false;
	bool bMuted = false;

	AStatusEffect()
	{
		for (int i = START_STAT_TYPE; i != END_STAT_TYPE; ++i)
		{
			stats.Add(nullptr);
		}
		for (int i = START_STAT_TYPE; i != END_STAT_TYPE; ++i)
		{
			mult_stats.Add(nullptr);
		}
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.TickInterval = 0.1f;
		PrimaryActorTick.bStartWithTickEnabled = true;
		PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	}

	~AStatusEffect()
	{
		UE_LOG(LogTemp, Warning, TEXT("Deleted Status Effect!"));
	}

	virtual void BeginPlay() override
	{
		
		Super::BeginPlay();
		SetActorTickEnabled(true);
		start_time = GetWorld()->GetTimeSeconds();
	}

	void SetOwner(AActor* in_owner)
	{
		owner = in_owner;
	}

	AActor* GetOwner() const
	{
		return owner;
	}

	void SetApplier(AActor* in_applier)
	{
		applier = in_applier;
	}

	AActor* GetApplier() const
	{
		return applier;
	}

	virtual float GetStat(StatType stat_type) const override
	{
		if (stats[int32(stat_type)])
		{
			return *stats[int32(stat_type)];
		}
		return 0;
	}

	virtual float GetStatMult(StatType stat_type) const override
	{
		if (mult_stats[int32(stat_type)])
		{
			return *mult_stats[int32(stat_type)];
		}
		return 1.0;
	}

	virtual bool SetStat(StatType stat_type, float in_stat) override
	{
		if (stats[int32(stat_type)])
		{
			*stats[int32(stat_type)] = in_stat;
			return true;
		}
		return false;
	}

	bool AddStat(StatType stat_type, float* in_stat = nullptr)
	{
		if (!in_stat)
		{
			stats[stat_type] = new float(0);
		}
		else
		{
			stats[stat_type] = in_stat;
		}
		return true;
	}

	bool AddStatMult(StatType stat_type, float* in_stat = nullptr)
	{
		if (!in_stat)
		{
			mult_stats[stat_type] = new float(1);
		}
		else
		{
			mult_stats[stat_type] = in_stat;
		}
		return true;
	}

	virtual void Tick(float dt) override;

	bool IsFinished() const
	{
		return bFinished;
	}

	void Refresh()
	{
		//current_duration = 0;
		//bFinished = false;
	}

	bool IsAura()
	{
		return bIsAura;
	}

	bool IsStackable()
	{
		return bStackable;
	}

	void SetTickTime(float in_time)
	{
		tick_time = int(in_time * 100);
	}

	UFUNCTION()
	virtual void StatusTick()
	{
		//UE_LOG(LogTemp, Warning, TEXT("AStatusEffect TICK"));
	}

	void RemoveConnection(StatusConnection* in_connection)
	{
		if (connections.find(in_connection) != connections.end())
		{
			connections.erase(in_connection);
		}
	}

	void SetAura(float in_radius)
	{
		bIsAura = true;
		aura_radius = in_radius;
	}

	void ApplyToStatusManager(class UStatusManager* in_manager);

	virtual void TickConnection(AActor* in_actor) {};

	virtual float GetMovespeed() { return 0; }
	virtual float GetHealth() { return 0; }
	virtual float GetMana() { return 0; }
	virtual float GetHealthRegen() { return 0; }
	virtual float GetManaRegen() { return 0; }
	virtual float GetArmor() { return 0; }
	virtual float GetAttackSpeed() { return 0; }
	virtual float GetMagicResist() { return 0; }
	virtual float GetAttackDamage() { return 0; }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGHSCLONE_API UStatusManager : public UActorComponent,
	public IStatInterface
{
	GENERATED_BODY()

	TArray<StatusConnection*> statuses;
	TMap<AStatusEffect*, float> linger_times;

public:	
	// Sets default values for this component's properties
	UStatusManager();

	virtual float GetStat(StatType stat_type) const override
	{
		float out_stat = 0;
		if (stat_type != StatMagicResist)
		{
			for (StatusConnection* si : statuses)
			{
				out_stat += si->effect->GetStat(stat_type);
			}
		}
		else
		{
			out_stat = 1.0;
			for (StatusConnection* si : statuses)
			{
				out_stat *= 1 - si->effect->GetStat(stat_type);
			}
			out_stat = 1 - out_stat;
		}
		return out_stat;
	}

	virtual float GetStatMult(StatType stat_type) const override
	{
		float out_stat = 1.0;
		
		{
			for (StatusConnection* si : statuses)
			{
				out_stat *= si->effect->GetStatMult(stat_type);
			}
		}

		return out_stat;
	}

	virtual bool SetStat(StatType stat_type, float in_stat) override
	{
		return false;
	}

	StatusConnection* AddStatus(AStatusEffect* in_status)
	{
		if (!GetOwner()->HasAuthority())
		{
			return nullptr;
		}
		in_status->SetOwner(GetOwner());
		StatusConnection* new_connection = nullptr;
		//if (!in_status->IsStackable())
		{
			bool found_existing = false;
			for (int i = 0; i < statuses.Num(); ++i)
			{
				if (in_status->GetClass() == statuses[i]->effect->GetClass() && !in_status->IsStackable())
				{
					//statuses[i]->last_refresh = GetWorld()->GetRealTimeSeconds();
					//statuses[i] = in_status;
					found_existing = true;
					break;
				}
				else if (in_status->GetApplier() == statuses[i]->effect->GetApplier()
					     && in_status->GetClass() == statuses[i]->effect->GetClass())
				{
					statuses[i]->last_refresh = GetWorld()->GetTimeSeconds();
					//statuses[i] = in_status;
					found_existing = true;
					break;
				}
			}
			if (!found_existing)
			{
				new_connection = new StatusConnection();
				new_connection->effect = in_status;
				new_connection->last_refresh = GetWorld()->GetTimeSeconds();
				new_connection->unit_with_status = GetOwner();
				statuses.Add(new_connection);
				UE_LOG(LogTemp, Warning, TEXT("Applied status effect to %s"),*GetOwner()->GetName());  
			}
		}
		/*
		if (!in_status->IsAura())
		{
			in_status->SetOwner(GetOwner());
		}
		else
		{
			linger_times.Add(in_status, 0.5);
		}
		statuses.Add(in_status);
		*/
		return new_connection;
	}

	StatusConnection* RefreshStatus(AStatusEffect* in_status)
	{
		StatusConnection* out_con = nullptr;
		bool found = false;
		for (auto& status : statuses)
		{
			if (status->effect == in_status)
			{
				status->last_refresh = GetWorld()->GetTimeSeconds();
				found = true;
			}
		}
		if (found == false)
		{
			out_con = AddStatus(in_status);
		}
		return out_con;
	}

	bool GetStunned() const
	{
		for (StatusConnection* si : statuses)
		{
			if (si->effect->bStunned)
			{
				return true;
			}
		}
		return false;
	}

	bool GetRooted() const
	{
		for (StatusConnection* si : statuses)
		{
			if (si->effect->bRooted)
			{
				return true;
			}
		}
		return false;
	}

	bool GetSilenced() const
	{
		for (StatusConnection* si : statuses)
		{
			if (si->effect->bSilenced)
			{
				return true;
			}
		}
		return false;
	}

	bool GetMuted() const
	{
		for (StatusConnection* si : statuses)
		{
			if (si->effect->bMuted)
			{
				return true;
			}
		}
		return false;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};


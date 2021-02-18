// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <unordered_map>
#include <map>
#include <unordered_set>

#include "CoreMinimal.h"
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
	float max_duration;
	float current_duration;
	int tick_time;
	TArray<float*> stats;
	TArray<float*> mult_stats;
	AActor* owner;
	AActor* applier;
	FTimerHandle tick_timer;
	static std::map<int, std::unordered_set<AStatusEffect*>> timer_status_map;
	static std::map<int, FTimerHandle> timer_map;

public:
	float applied_time;
	float last_tick = -100.0;
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
		current_duration = 0;
		applied_time = 0;
		
	}

	~AStatusEffect()
	{
		UE_LOG(LogTemp, Warning, TEXT("Deleted Status Effect!"));
		timer_status_map[tick_time].erase(this);
	}

	virtual void BeginPlay()
	{
		if (timer_status_map.find(tick_time) == timer_status_map.end())
		{
			FTimerHandle new_handle;
			GetWorldTimerManager().SetTimer(new_handle, this, &AStatusEffect::StatusTick, float(tick_time*1.0/100.0), false);
			timer_map[tick_time] = new_handle;
			timer_status_map[tick_time].insert(this);
		}
		else
		{

		}
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

	virtual void TickStatus(float dt) 
	{
		applied_time += dt;
		if (!bIsAura)
		{
			current_duration += dt;
			if (current_duration >= max_duration)
			{
				bFinished = true;
			}
		}
	};

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
		UE_LOG(LogTemp, Warning, TEXT("AStatusEffect TICK"));
	}

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

	TArray<AStatusEffect*> statuses;
	TMap<AStatusEffect*, float> linger_times;

public:	
	// Sets default values for this component's properties
	UStatusManager();

	virtual float GetStat(StatType stat_type) const override
	{
		float out_stat = 0;
		if (stat_type != StatMagicResist)
		{
			for (IStatInterface* si : statuses)
			{
				out_stat += si->GetStat(stat_type);
			}
		}
		else
		{
			out_stat = 1.0;
			for (IStatInterface* si : statuses)
			{
				out_stat *= 1 - si->GetStat(stat_type);
			}
			out_stat = 1 - out_stat;
		}
		return out_stat;
	}

	virtual float GetStatMult(StatType stat_type) const override
	{
		float out_stat = 1.0;
		
		{
			for (IStatInterface* si : statuses)
			{
				out_stat *= si->GetStatMult(stat_type);
			}
		}

		return out_stat;
	}

	virtual bool SetStat(StatType stat_type, float in_stat) override
	{
		return false;
	}

	bool AddStatus(AStatusEffect* in_status)
	{
		in_status->SetOwner(GetOwner());

		//if (!in_status->IsStackable())
		{
			bool found_existing = false;
			for (int i = 0; i < statuses.Num(); ++i)
			{
				if (in_status->GetClass() == statuses[i]->GetClass() && !in_status->IsStackable())
				{
					in_status->applied_time = statuses[i]->applied_time;
					in_status->last_tick = statuses[i]->last_tick;
					//statuses[i] = in_status;
					found_existing = true;
					break;
				}
				else if (in_status->GetApplier() == statuses[i]->GetApplier())
				{
					in_status->applied_time = statuses[i]->applied_time;
					in_status->last_tick = statuses[i]->last_tick;
					//statuses[i] = in_status;
					found_existing = true;
					break;
				}
			}
			if (!found_existing)
			{

				statuses.Add(in_status);
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
		return true;
	}

	bool RefreshStatus(AStatusEffect* in_status)
	{
		bool found = false;
		for (auto& status : statuses)
		{
			if (status == in_status)
			{
				linger_times[status] = 0.5;
				found = true;
			}
		}
		if (found == false)
		{
			AddStatus(in_status);
		}
		return found;
	}

	bool GetStunned() const
	{
		for (AStatusEffect* si : statuses)
		{
			if (si->bStunned)
			{
				return true;
			}
		}
		return false;
	}

	bool GetRooted() const
	{
		for (AStatusEffect* si : statuses)
		{
			if (si->bRooted)
			{
				return true;
			}
		}
		return false;
	}

	bool GetSilenced() const
	{
		for (AStatusEffect* si : statuses)
		{
			if (si->bSilenced)
			{
				return true;
			}
		}
		return false;
	}

	bool GetMuted() const
	{
		for (AStatusEffect* si : statuses)
		{
			if (si->bMuted)
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


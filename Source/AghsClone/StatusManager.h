// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatInterface.h"
#include "StatusManager.generated.h"

UCLASS(Blueprintable)
class AGHSCLONE_API UStatusEffect : public UObject,
	public IStatInterface
{
	GENERATED_BODY()
protected:
	bool bFinished = false;
	bool bIsAura = false;
	float max_duration;
	float current_duration;
	TArray<float*> stats;
	TArray<float*> mult_stats;
	AActor* owner;

public:

	bool bStunned = false;
	bool bRooted = false;
	bool bSilenced = false;
	bool bMuted = false;

	UStatusEffect()
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
	}

	~UStatusEffect()
	{
		UE_LOG(LogTemp, Warning, TEXT("Deleted Status Effect!"));
	}

	void SetOwner(AActor* in_owner)
	{
		owner = in_owner;
	}

	AActor* GetOwner() const
	{
		return owner;
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

	TArray<UStatusEffect*> statuses;
	TMap<UStatusEffect*, float> linger_times;

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

	bool AddStatus(UStatusEffect* in_status)
	{
		if (!in_status->IsAura())
		{
			in_status->SetOwner(GetOwner());
		}
		else
		{
			linger_times.Add(in_status, 0.5);
		}
		statuses.Add(in_status);
		return true;
	}

	bool RefreshStatus(UStatusEffect* in_status)
	{
		bool found = false;
		for (auto& status : statuses)
		{
			if (status == in_status)
			{
				linger_times[status] = 0.5;
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
		for (UStatusEffect* si : statuses)
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
		for (UStatusEffect* si : statuses)
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
		for (UStatusEffect* si : statuses)
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
		for (UStatusEffect* si : statuses)
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


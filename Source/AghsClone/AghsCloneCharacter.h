// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <vector>
#include <deque>

#include "Containers/Queue.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"

#include "Ability.h"
#include "Components/DecalComponent.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "CommandInterface.h"
#include "VisionInterface.h"
#include "FieldActorInterface.h"
#include "AttackProjectile.h"
#include "WalletComponent.h"
#include "InventoryComponent.h"
#include "AbilityContainerInterface.h"
#include "StatInterface.h"
#include "ExperienceInterface.h"
#include "StatusManager.h"
#include "AghsCloneCharacter.generated.h"

UENUM()
enum CharacterState
{
    Idle UMETA(DisplayName = "Idle"),
    Walking UMETA(DisplayName = "Walking"),
    Casting UMETA(DisplayName = "Casting"),
    AttackPoint UMETA(DisplayName = "AttackPoint"),
    AttackBackswing UMETA(DisplayName = "AttackBackswing"),
    Stunned UMETA(DisplayName = "Stunned"),
    Channeling UMETA(DisplayName = "Channeling"),
};


UCLASS(Blueprintable)
class AAghsCloneCharacter : public ACharacter, 
	public IHealthInterface,
	public IManaInterface,
	public ICommandInterface,
	public IVisionInterface,
	public IFieldActorInterface,
	public IAbilityContainerInterface,
	public IStatInterface,
	public IExperienceInterface
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	float MaxHealth;
	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float HealthRegen;
	UPROPERTY(Replicated)
	float Armor;
	float MagicResist;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float ManaRegen;
	UPROPERTY(Replicated)
	float MaxMana;
	UPROPERTY(Replicated)
	float Experience;
	UPROPERTY(Replicated)
	int32 Level = 1;
	UPROPERTY(Replicated)
	float AttackProjectileSpeed;

	bool IsSpellImmune;
	bool IsAttackImmune;
    bool IsStunned;
    bool IsRooted;
    bool IsSilenced;
    bool IsMuted;


	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 team;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* unit_owner;
	float vision_radius;
	float attack_range;
	float attack_damage;
	float BaseAttackTime;
	float AttackSpeed;
	float BaseMovespeed;
	double last_attack_time;
	TArray<float*> stats;

protected:


public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BaseAttackPoint;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BaseAttackBackswing;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<CharacterState> char_state;
	float char_state_time;

	AAghsCloneCharacter();

	// Called every frame.
	// UFUNCTION(reliable, server)
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	/** Returns VisionLight subobject **/
	FORCEINLINE class UPointLightComponent* GetVisionLight() { return VisionLight; }

	virtual float GetStat(StatType stat_type) const override
	{
		float out_stat = 0;
		float out_mult = 1.0;
		if (stat_type != StatMagicResist)
		{
			if (stats[stat_type])
			{
				out_stat += *stats[stat_type];
			}
			for (auto& si : StatInterfaces)
			{
				out_stat += si->GetStat(stat_type);
				out_mult *= si->GetStatMult(stat_type);
			}
		}
		else
		{
			out_stat = 1 - out_stat;
			if (stats[stat_type])
			{
				out_stat *= 1 - *stats[stat_type];
			}
			for (auto& si : StatInterfaces)
			{
				out_stat *= 1 - si->GetStat(stat_type);
			}
			out_stat = 1 - out_stat;
		}
		return out_mult*out_stat;
	}

	virtual bool SetStat(StatType stat_type, float in_stat) override
	{
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

	int32 GetTeam()
	{
		return team;
	}

	void SetTeam(int32 in_team)
	{
		team = in_team;
	}

	AActor* GetUnitOwner()
	{
		return unit_owner;
	}

	void SetUnitOwner(AActor* in_owner)
	{
		unit_owner = in_owner;
	}

	// Called when the game starts or when spawned
	virtual void BeginPlay()
	{
		Super::BeginPlay();
		PrimaryActorTick.bCanEverTick = (GetLocalRole() == ROLE_Authority);
	}

	void SetSelected(bool is_selected)
	{
		//CursorToWorld->SetVisibility(is_selected);
	}

	// HEALTH INTERFACE IMPLEMENTATION
	virtual float GetHealth() const override
	{
		return Health*GetMaxHealth();
	}

	virtual float GetHealthRegen() const override
	{
		return GetStat(StatHealthRegen);// +StatusManager->GetHealthRegen();
	}

	virtual float GetDelayedHealth() const override
	{
		return Health*GetMaxHealth();
	}

	virtual void SetHealth(float in_val) override
	{
		Health = std::min(GetMaxHealth(), std::max(0.0f, in_val))*1.0/GetMaxHealth();
		if (Health <= 0.01)
		{
			
			TSet<AActor*> experience_range;
			TArray<AAghsCloneCharacter*> experience_range_valid;
			temp_sphere->SetSphereRadius(1200, true);
			temp_sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			temp_sphere->GetOverlappingActors(experience_range, AAghsCloneCharacter::StaticClass());
			temp_sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			temp_sphere->SetSphereRadius(0.1, true);
			

			for (auto& act : experience_range)
			{
				auto aghs = Cast<AAghsCloneCharacter>(act);
				if (aghs->GetTeam() != GetTeam())
				{
					experience_range_valid.Add(aghs);
				}
			}
			float xp_distribution = Bounty->GetXpBounty() / experience_range_valid.Num();
			for (auto& character : experience_range_valid)
			{
				auto xp_int = Cast<IExperienceInterface>(character);
				xp_int->GiveExperience(xp_distribution);
			}
			Destroy();
		}
		
	}

	virtual void SetMaxHealth(float in_val)
	{
		MaxHealth = in_val;
	}

	virtual float GetArmor() const override
	{
		return GetStat(StatArmor);
	}

	virtual float GetMagicResist() const override
	{
		return GetStat(StatMagicResist);// + Inventory->GetMagicResist();// + StatusManager->GetMagicResist();
	}

	virtual float GetMaxHealth() const override
	{
		return GetStat(StatMaxHealth);// MaxHealth + Inventory->GetHealth();// + StatusManager->GetMaxHealth();
	}
	// END HEALTH INTERFACE

	virtual void SetArmor(float in_armor)
	{
		Armor = in_armor;
	}

	virtual void SetAttackDamage(float in_damage)
	{
		attack_damage = in_damage;
	}

	virtual float GetMovespeed() const
	{
		return GetStat(StatMovespeed);// BaseMovespeed + Inventory->GetMovespeed();// + StatusManager->GetMovespeed();
	}

	virtual void SetBaseMovespeed(float in_movespeed)
	{
		BaseMovespeed = in_movespeed;
	}

	// MANA INTERFACE IMPLEMENTATION
	virtual float GetMana()
	{
		return Mana*GetMaxMana();
	}

	virtual float GetManaRegen() const override
	{
		return GetStat(StatManaRegen);// + StatusManager->GetManaRegen();
	}

	virtual void SetMana(float in_val)
	{
		Mana = std::min(GetMaxMana(), std::max(0.0f, in_val))/GetMaxMana();
	}

	virtual void SetMaxMana(float in_val)
	{
		MaxMana = in_val;
	}

	virtual float GetMaxMana() const
	{
		return GetStat(StatMaxMana);// + StatusManager->GetMana();
	}
	// END MANA INTERFACE

	// AUTO ATTACK IMPLEMENTATION

	void SetAttackProjectileSpeed(float in_speed)
	{
		AttackProjectileSpeed = in_speed;
	}

	bool AttackActor(AActor* in_target)
	{
		FDateTime t = FDateTime::UtcNow();
		double ts = t.ToUnixTimestamp() + t.GetMillisecond() * 1.0 / 1000;
		float attack_time = GetAttackTime();
		double t_diff = ts - last_attack_time;
		if (t_diff >= attack_time)
		{
			auto proj = GetWorld()->SpawnActor<AAttackProjectile>();
			proj->SetActorLocation(GetActorLocation());
			proj->SetTarget(in_target);
			proj->SetShooter(this);
			proj->SetSpeed(AttackProjectileSpeed);
			ResetAttackTimer(ts);
			return true;
		}
		return false;
	}

    void ApplyOnHit(AAghsCloneCharacter* other_char)
    {
        DamageInstance attack_damage_instance;
		attack_damage_instance.value = GetAttackDamage();
		attack_damage_instance.damage_type = PhysicalDamage;
		attack_damage_instance.is_attack = true;
		other_char->ApplyDamage(attack_damage_instance, this);
		for (auto& ab : Abilities)
        {
            if (ab->bOnHit)
            {
                ab->OnHit(attack_damage_instance, other_char);
            }
        }
        Inventory->OnHit(attack_damage_instance, other_char);
    }

	float GetAttackDamage() const
	{
		return GetStat(StatAttackDamage);// + StatusManager->GetAttackDamage();
	}

	float GetAttackSpeed() const
	{
		return AttackSpeed + Inventory->GetAttackSpeed();// + StatusManager->GetAttackSpeed();
	}

	float GetAttackTime() const
	{
		float attack_time = BaseAttackTime / (GetAttackSpeed() * 0.01);
		return attack_time;
	}

	UFUNCTION( BlueprintCallable )
    float GetAttackPoint() const
    {
        return BaseAttackPoint/(1+(GetAttackSpeed() - 100));
    }

	UFUNCTION( BlueprintCallable )
    float GetAttackBackswing() const
    {
        return BaseAttackBackswing/(1+(GetAttackSpeed() - 100));
    }

	void ResetAttackTimer(double in_time)
	{
		last_attack_time = in_time;
	}

	// END AUTO ATTACK IMPLEMENTATION

	// EXPERIENCE INTERFACE IMPLEMENTATION

	virtual void GiveExperience(float in_exp) override
	{
		Experience += in_exp;
		int32 temp_level = 0;
		for (int i = 0; i < level_thresholds.size(); ++i)
		{
			if (Experience > level_thresholds[i])
			{
				temp_level = i;
				if (i != level_thresholds.size() - 1)
				{
					if (Experience < level_thresholds[i + 1])
					{
						break;
					}
				}
			}
		}
		
		Level = temp_level + 1;
	}

	virtual float GetExperience() const override
	{
		return Experience;
	}

	virtual int32 GetLevel() const override
	{
		return Level;
	}

	// END EXPERIENCE INTERFACE

	void SetTargetingActive(int32 ability_num)
	{
		int count = 1;
		for (auto& ab : Abilities)
		{
			if (ability_num != count)
			{
				//ab->TargetingDecal->SetVisibility(false);
			}
			else
			{
				ab->TargetingDecal->SetVisibility(true);
			}
			count += 1;
		}
		//CursorToWorld->SetVisibility(false);
		//Abilities[ability_num]->TargetingDecal->SetVisibility(true);
		//targeting_active = ability_num;
	}

	int32 GetTargetingActive()
	{
		return targeting_active;
	}

	FVector GetDestination()
	{
		return current_destination;
	}

	void SetDestination(FVector in_destination)
	{
		current_destination = in_destination;
	}

    TArray<UAbility*>& GetAbilityArray()
    {
        return Abilities;
    }

    virtual IAbilityInterface* GetAbility(int32 ability_num) const
    {
        if (ability_num < 4)
        {
            return Cast<IAbilityInterface>(Abilities[ability_num]);
        }
        else if (ability_num < 10)
        {
            return Cast<IAbilityInterface>(Inventory->GetAbility(ability_num - 4));
        }
		return nullptr;
    }

	

	bool TriggerTargetedAbility(FVector target_loc)
	{
		bool retval = false;
		if (targeting_active >= 0 && targeting_active < Abilities.Num())
		{
			if ((GetActorLocation() - target_loc).Size() < Abilities[targeting_active]->CastRange)
			{
				Abilities[targeting_active]->OnGroundActivationMeta(target_loc);
				retval = true;
			}
			Abilities[targeting_active]->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}
		targeting_active = -1;
		return retval;
	}

	bool TriggerTargetedAbility(int32 ability_num, FVector target_loc)
	{
		bool retval = false;
        auto ability = GetAbility(ability_num);
		if (ability)
		{
			if ((GetActorLocation() - target_loc).Size() < ability->GetCastRange())
			{
				ability->OnGroundActivationMeta(target_loc);
				retval = true;
			}
			//ability->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}
		return retval;
	}

	bool TriggerTargetedAbility(int32 ability_num, AActor* unit)
	{
		bool retval = false;
		
		auto ability = GetAbility(ability_num);
		if (ability)
		{
			if ((GetActorLocation() - unit->GetActorLocation()).Size() < ability->GetCastRange())
			{
				ability->OnUnitActivationMeta(unit);
				retval = true;
			}
			//ability->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}
		
		return retval;
	}

	bool TriggerAbility(int32 ability_num)
	{
		bool retval = false;

		auto ability = GetAbility(ability_num);
		if (ability)
		{
			ability->OnActivationMeta();
			retval = true;
			//ability->TargetingDecal->SetVisibility(false);
		}
		else
		{
			retval = false;
		}

		return retval;
	}

	// COMMAND INTERFACE IMPLEMENTATION

	//UFUNCTION(reliable, server)
	virtual void QueueCommand(const FCommand& in_command) override
	{
		UE_LOG(LogTemp, Warning, TEXT("Command Queued, %d"), command_queue.IsEmpty());
		command_queue.Enqueue(in_command);
	}

	//UFUNCTION(reliable, server, WithValidation)
	virtual void ClearCommandQueue() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Command Queue Cleared"));
		command_queue.Empty();
	}

	//UFUNCTION(reliable, server)
	virtual void SetCommand(const FCommand& in_command) override
	{
		ClearCommandQueue();
		current_command = in_command;
	}

	virtual void NextCommand() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Command"));
		if (!command_queue.IsEmpty())
		{
			command_queue.Dequeue(current_command);
			destination_set = false;
		}
		else
		{
			current_command.command_type = NONE;
		}
	}

	virtual FVector GetCurrentDestination() const override
	{
		return current_destination;
	}

	virtual void SetCurrentDestination(FVector in_dest) override
	{
		current_destination = in_dest;
	}

	virtual FCommand GetCurrentCommand() const override
	{
		return current_command;
	}

	virtual FCommand GetLastCommand() const override
	{
		return LastCommand;
	}

	virtual void SetLastCommand(const FCommand& in_command) override
	{
		LastCommand = in_command;
	}

	void ProcessAttackMove(const FCommand& in_command, float dt);

	void ProcessAbilityCommand(const FCommand& in_command, float dt);
	// END COMMAND INTERFACE

	// VISION INTERFACE IMPLEMENTATION
	virtual float GetVisionRadius() const override
	{
		return vision_radius;
	}
    // END VISION INTERFACE

    virtual IAbilityInterface* GetChanneled()
    {
        return ChanneledAbility;
    }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AAghsCloneCharacter, Health);
		DOREPLIFETIME(AAghsCloneCharacter, HealthRegen);
		DOREPLIFETIME(AAghsCloneCharacter, MaxHealth);
		DOREPLIFETIME(AAghsCloneCharacter, MaxMana);
		DOREPLIFETIME(AAghsCloneCharacter, Mana);
		DOREPLIFETIME(AAghsCloneCharacter, ManaRegen);
		DOREPLIFETIME(AAghsCloneCharacter, team);
		DOREPLIFETIME(AAghsCloneCharacter, unit_owner);
		DOREPLIFETIME(AAghsCloneCharacter, Inventory);
		DOREPLIFETIME(AAghsCloneCharacter, Wallet);
		DOREPLIFETIME(AAghsCloneCharacter, Level);
		DOREPLIFETIME(AAghsCloneCharacter, Experience);
		DOREPLIFETIME(AAghsCloneCharacter, AttackProjectileSpeed);
	}

	TArray<UAbility*> Abilities;
	TArray<IStatInterface*> StatInterfaces;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* VisionLight;

	/** A decal that projects to the cursor location. */
	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* Inventory;

	UPROPERTY( Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UWalletComponent* Wallet;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBountyComponent* Bounty;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UStatusManager* StatusManager;

	class USphereComponent* temp_sphere;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	int32 targeting_active;

	TQueue<FCommand> command_queue;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCommand current_command;
	FCommand LastCommand;
    IAbilityInterface* ChanneledAbility;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	FVector current_destination;
	bool destination_set;
};


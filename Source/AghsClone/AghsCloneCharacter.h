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
#include "AghsCloneCharacter.generated.h"

class CommonUnitStats
{
public:
	CommonUnitStats():
		MaxHealth(100.0),
		HealthRegeneration(0.1),
		Armor(0),
		MagicResist(0.25),
		IsSpellImmune(false),
		IsAttackImmune(false),
		MaxMana(100.0)
	{ 
		Health = 1.0; 
		Mana = MaxMana;
	}
	UPROPERTY(Replicated)
	float MaxHealth;
	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float HealthRegeneration;
	UPROPERTY(Replicated)
	float Armor;
	float MagicResist;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float MaxMana;
	bool IsSpellImmune;
	bool IsAttackImmune;
};

UCLASS(Blueprintable)
class AAghsCloneCharacter : public ACharacter, 
	public IHealthInterface,
	public IManaInterface,
	public ICommandInterface,
	public IVisionInterface,
	public IFieldActorInterface,
	public IAbilityContainerInterface
	//public CommonUnitStats
	
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	float MaxHealth;
	UPROPERTY(Replicated)
	float Health;
	UPROPERTY(Replicated)
	float HealthRegeneration;
	UPROPERTY(Replicated)
	float Armor;
	float MagicResist;
	UPROPERTY(Replicated)
	float Mana;
	UPROPERTY(Replicated)
	float MaxMana;
	bool IsSpellImmune;
	bool IsAttackImmune;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 team;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* unit_owner;
	float vision_radius;
	float attack_range;
	float attack_damage;
	float BaseAttackTime;
	float InitialAttackSpeed;
	float AttackSpeed;
	float BaseMovespeed;
	double last_attack_time;


public:
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
		CursorToWorld->SetVisibility(is_selected);
	}

	// HEALTH INTERFACE IMPLEMENTATION
	virtual float GetHealth() const override
	{
		return Health*GetMaxHealth();
	}

	virtual float GetDelayedHealth() const override
	{
		return Health*GetMaxHealth();
	}

	virtual void SetHealth(float in_val) override
	{
		Health = std::min(GetMaxHealth(), std::max(0.0f, in_val))*1.0/GetMaxHealth();
		if (Health == 0)
		{
			Destroy();
		}
	}

	virtual void SetMaxHealth(float in_val)
	{
		MaxHealth = in_val;
	}

	virtual float GetArmor() const override
	{
		return Armor;
	}

	virtual float GetMagicResist() const override
	{
		return MagicResist;
	}

	virtual float GetMaxHealth() const override
	{
		return MaxHealth + Inventory->GetHealth();
	}
	// END HEALTH INTERFACE

	virtual float GetMovespeed() const
	{
		return BaseMovespeed + Inventory->GetMovespeed();
	}

	// MANA INTERFACE IMPLEMENTATION
	virtual float GetMana()
	{
		return Mana*GetMaxMana();
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
		return MaxMana + Inventory->GetMana();;
	}
	// END MANA INTERFACE

	// AUTO ATTACK IMPLEMENTATION

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
		other_char->ApplyDamage(attack_damage_instance);
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
		return attack_damage + Inventory->GetAttackDamage();
	}

	float GetAttackSpeed() const
	{
		return InitialAttackSpeed + AttackSpeed + Inventory->GetAttackSpeed();
	}

	float GetAttackTime() const
	{
		float attack_time = BaseAttackTime / (GetAttackSpeed() * 0.01);
		return attack_time;
	}

	void ResetAttackTimer(double in_time)
	{
		last_attack_time = in_time;
	}

	// END AUTO ATTACK IMPLEMENTATION

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

    virtual IAbilityInterface* GetAbility(int32 ability_num)
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

	void ProcessAttackMove(const FCommand& in_command, float dt);

	void ProcessAbilityCommand(const FCommand& in_command, float dt);
	// END COMMAND INTERFACE

	// VISION INTERFACE IMPLEMENTATION
	virtual float GetVisionRadius() const override
	{
		return vision_radius;
	}

	

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AAghsCloneCharacter, Health);
		DOREPLIFETIME(AAghsCloneCharacter, MaxHealth);
		DOREPLIFETIME(AAghsCloneCharacter, MaxMana);
		DOREPLIFETIME(AAghsCloneCharacter, Mana);
		DOREPLIFETIME(AAghsCloneCharacter, team);
		DOREPLIFETIME(AAghsCloneCharacter, unit_owner);
		DOREPLIFETIME(AAghsCloneCharacter, Inventory);
		DOREPLIFETIME(AAghsCloneCharacter, Wallet);
	}

	TArray<UAbility*> Abilities;

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
	

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	int32 targeting_active;

	TQueue<FCommand> command_queue;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FCommand current_command;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	FVector current_destination;
	bool destination_set;
};


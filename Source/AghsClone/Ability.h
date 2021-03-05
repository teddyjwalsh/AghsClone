// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/ShapeComponent.h"
#include "Net/UnrealNetwork.h"
#include "ManaInterface.h"
#include "HealthInterface.h"
#include "FieldActorInterface.h"
#include "StatusManager.h"
#include "AbilityInterface.h"
#include "Ability.generated.h"

UCLASS()
class AGHSCLONE_API AAbilityInstance : public AActor,
    public IFieldActorInterface
{
    GENERATED_BODY()

    float scale = 0.1;
    float hit_damage = 100;
    float projectile_speed = 0;
    FVector projectile_vector;
    bool targeted_projectile = false;
    bool distance_projectile = false;
    bool enabled = true;
    bool tick_hit_enabled = false;
    bool first_tick = true;
    bool reset_hit_on_tick = false;
    bool apply_standard_status = false;
    bool apply_standard_damage = false;
    DamageType damage_type = MagicDamage;
    //std::set<int32> hit;
    TSet<AActor*> hit;
    UShapeComponent* overlapper;
    UPROPERTY()
    UClass* status_effect_class;

public:
    AActor* projectile_target;
    int team;
    // Sets default values for this actor's properties
    //AAbilityInstance();
    void SetStatusEffect(UClass* in_status_effect)
    {
        status_effect_class = in_status_effect;
    }

    void SetEnabled(bool in_enable)
    {
        enabled = in_enable;
    }

    void SetTickHitEnabled(bool in_enable)
    {
        tick_hit_enabled = in_enable;
    }

    void SetResetHitOnTick(bool in_enable)
    {
        reset_hit_on_tick = in_enable;
    }

    void SetHitDamage(float in_damage)
    {
        hit_damage = in_damage;
    }

    float GetHitDamage() const
    {
        return hit_damage;
    }

    void SetDamageType(DamageType in_type)
    {
        damage_type = in_type;
    }

    void SetOverlapper(UShapeComponent* in_shape)
    {
        overlapper = in_shape;
    }

    void SetApplyStandardStatus(bool in_enable)
    {
        apply_standard_status = in_enable;
    }

    void EnableUnitTargetedProjectile(AActor* targeted_unit)
    {
        projectile_target = targeted_unit;
        targeted_projectile = true;
        overlapper->OnComponentBeginOverlap.AddDynamic(this, &AAbilityInstance::OnOverlap);
    }

    void EnableDistanceProjectile(FVector in_vector)
    {
        projectile_vector = in_vector;
        distance_projectile = true;
        overlapper->OnComponentBeginOverlap.AddDynamic(this, &AAbilityInstance::OnOverlap);
    }

    void SetProjectileSpeed(float in_speed)
    {
        projectile_speed = in_speed;
    }

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        SetActorTickInterval(0.1);

    }

public:


    virtual void OnNewHit(AActor* hit_char)
    {

    }

    virtual void OnHit(AActor* hit_char)
    {

    }

    // Called every frame
    virtual void Tick(float DeltaTime) override
    {
        Super::Tick(DeltaTime);
        TSet<AActor*> set;
        overlapper->GetOverlappingActors(set);

        if (targeted_projectile)
        {
            if (IsValid(projectile_target))
            {
                if (!projectile_target->IsPendingKill())
                {
                    FVector move_vector = projectile_target->GetActorLocation() - GetActorLocation();
                    move_vector.Normalize();
                    FHitResult hit_result;
                    SetActorLocation(GetActorLocation() + move_vector * projectile_speed * DeltaTime, true, &hit_result);
                    if (hit_result.IsValidBlockingHit())
                    {
                        OnHit(hit_result.GetActor());
                    }
                }
            }
            else
            {
                Destroy();
            }
        }
        
        if (distance_projectile)
        {
            FVector move_vector = projectile_vector;
            move_vector.Normalize();
            SetActorLocation(GetActorLocation() + move_vector * projectile_speed * DeltaTime);
        }

        if (enabled && tick_hit_enabled)
        {
            if (set.Num() > 0 && HasAuthority())
            {
                for (auto& act : set)
                {
                    int32 act_id = act->GetUniqueID();
                    OnHit(act);
                    if (!hit.Find(act) && act != GetOwner())
                    {

                        {
                            auto health_comp = Cast<IHealthInterface>(act);
                            OnNewHit(act);
                            hit.Add(act);// (act->GetUniqueID());
                            if (health_comp && apply_standard_damage)
                            {
                                DamageInstance shock_damage;
                                shock_damage.value = hit_damage;
                                shock_damage.damage_type = damage_type;
                                shock_damage.is_attack = false;
                                health_comp->ApplyDamage(shock_damage, GetOwner());
                                
                                UE_LOG(LogTemp, Warning, TEXT("SHOCK HIT CHARACTER: %f, %d"), health_comp->GetHealth(), act);
                            }
                            auto status_manager = Cast<UStatusManager>(act->GetComponentByClass(UStatusManager::StaticClass()));
                            if (status_manager && apply_standard_status)
                            {
                            }
                        }
                    }
                }
                if (reset_hit_on_tick)
                {
                    hit.Empty();
                }
            }
        }

    }

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
    {
        OnHit(OtherActor);
    }
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGHSCLONE_API UAbility : public UActorComponent,
    public IAbilityInterface
{
	GENERATED_BODY()

    static TMap<FString, UTexture2D*> materials;

    UPROPERTY(Replicated)
    UTexture2D* MyMat;
    UPROPERTY()
    UClass* status_effect_class;


public:	
	// Sets default values for this component's properties
	UAbility();
    void SetStatusEffect(UClass* in_status_effect)
    {
        status_effect_class = in_status_effect;
    }

protected:
    int charges;
    int max_charges;

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	bool bUnitTargeted = false;
	bool bGroundTargeted = false;
	bool bToggleable = false;
	bool bToggled = false;
	bool bPassive = false;
    bool bOnHit = false;
    bool bIsUltimate = false;
    bool bIsChanneled = false;
    bool bIsCharges = false;
	float DefaultCooldown;
    UPROPERTY( Replicated )
    float MaxChannelTime;
    UPROPERTY( Replicated )
    float ChannelTime;
    UPROPERTY(Replicated)
       bool bDoneChanneling;
    UPROPERTY( Replicated )
	float ManaCost;
	float CastRange;
    UPROPERTY(Replicated)
    float Cooldown;    
    UPROPERTY(Replicated)
    float TickInterval;

    std::vector<float> Cooldowns;
    std::vector<float> ManaCosts;
    std::vector<float> Damages;
    UPROPERTY(Replicated)
    float CurrentCooldown;
	class UDecalComponent* TargetingDecal;
    UPROPERTY()
    int32 max_level;
    UPROPERTY(Replicated)
    int32 current_level;
    UPROPERTY(Replicated)
    float CastPoint;
    UPROPERTY(Replicated)
    float CastBackswing;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UAbility, ManaCost);
        DOREPLIFETIME(UAbility, Cooldown);
        DOREPLIFETIME(UAbility, ChannelTime);
        DOREPLIFETIME(UAbility, MyMat);
        DOREPLIFETIME(UAbility, CurrentCooldown);
        DOREPLIFETIME(UAbility, current_level);
    }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual float GetMaxLevel() const
    {
        return max_level;
    }
    virtual float GetLevel() const
    {
        return current_level;
    }
    virtual bool LevelUp()
    {
        if (current_level < max_level)
        {
            current_level += 1;
            return true;
        }
        return false;
    }
    virtual float GetCooldown() const override
    {
        if ((current_level - 1) < Cooldowns.size())
        {
            return Cooldowns[current_level - 1];
        }
        return Cooldowns[Cooldowns.size() - 1];
    }
    virtual float GetCurrentCooldown() const override 
    {
        return CurrentCooldown;
    }
    virtual void SetCurrentCooldown(float in_val) override
    {
        CurrentCooldown = in_val;
    }
    virtual float GetCastRange() const
    {
        return CastRange;
    }
	virtual float GetManaCost() const override 
    {
        if ((current_level - 1) < ManaCosts.size())
        {
            return ManaCosts[current_level - 1];
        }
        return ManaCosts[ManaCosts.size() - 1];
    }
    virtual float GetDamage() const override
    {
        if ((current_level - 1) < ManaCosts.size())
        {
            return Damages[current_level - 1];
        }
        return Damages[ManaCosts.size() - 1];
    }
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(AActor* target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
    virtual void TickChannel(float DeltaTime) {}
    virtual float GetChannelTime() const override { return ChannelTime; }
    virtual float GetMaxChannelTime() const override { return MaxChannelTime; }
    virtual bool IsDoneChanneling() const override { return bDoneChanneling; }
    virtual bool IsChanneling() const override { return ChannelTime > 0; }
	virtual void OnGroundActivation(const FVector& target)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("Ground-targeted activation, %f, %f"), target.X, target.Y); 
	}
    virtual bool IsToggleable() const override
    {
        return bToggleable;
    }
    virtual bool IsToggled() const override
    {
        return bToggled;
    }
    virtual bool IsPassive() const override
    {
        return bPassive;
    }
    virtual bool IsUnitTargeted() const override
    {
        return bUnitTargeted;
    }
    virtual bool IsGroundTargeted() const override
    {
        return bGroundTargeted;
    }
    virtual void Toggle() override
    {
        bToggled = !bToggled;
    }
    virtual bool IsUltimate() const
    {
        return bIsUltimate;
    }
    virtual bool IsChanneled() const
    {
        return bIsChanneled;
    }
    virtual bool CanBeLeveled(int32 in_level)
    {
        if (bIsUltimate)
        {
            if (in_level >= 18)
            {
                return true;
            }
            else if (in_level >= 12 && GetLevel() <= 1)
            {
                return true;
            }
            else if (in_level >= 6 && GetLevel() <= 0)
            {
                return true;
            }
        }
        else
        {
            if (in_level >= 7)
            {
                return true;
            }
            else if (in_level >= 5 && GetLevel() < 3)
            {
                return true;
            }
            else if (in_level >= 3 && GetLevel() < 2)
            {
                return true;
            }
            else if (in_level >= 1 && GetLevel() < 1)
            {
                return true;
            }
        }
        return false;
    }
    void SetMaterial(FString to_load)
    {
        if (!materials.Find(to_load))
        {
            //ConstructorHelpers::FObjectFinder<UTexture2D> ScreenMat(*FString::Printf(TEXT("Texture2D'/Game/Textures/%s.%s'"), *to_load, *to_load));
            auto item_tex = Cast<UTexture2D>(
                StaticLoadObject(UTexture2D::StaticClass(),
                    NULL,
                    *FString::Printf(TEXT("/Game/Textures/%s.%s"),
                        *to_load,
                        *to_load)));
            if (item_tex)
            {
                materials.Add(to_load, item_tex);
                MyMat = materials[to_load];
            }
        }
        else
        {
            MyMat = materials[to_load];
        }
        
    }

    UTexture2D* GetMaterial()
    {
        return MyMat;
    }

    virtual float GetCastPoint() const
    {
        return CastPoint;
    }

    virtual float GetCastBackswing() const
    {
        return CastBackswing;
    }

    void SetTickInterval(float tick_interval)
    {
        TickInterval = tick_interval;
    }

    virtual bool IsCharges() const override
    {
        return bIsCharges;
    }

    virtual bool UseCharge() override
    {
        if (charges)
        {
            charges -= 1;
            return true;
        }
        return false;
    }

    virtual int GetChargeCount() const override
    {
        return charges;
    }

    virtual int GetMaxCharges() const override
    {
        return max_charges;
    }
};

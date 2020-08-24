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
    bool enabled = true;
    bool tick_hit_enabled = false;
    DamageType damage_type = MagicDamage;
    //std::set<int32> hit;
    TSet<AActor*> hit;
    UShapeComponent* overlapper;
    UPROPERTY()
    UClass* status_effect_class;


public:
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

    void SetHitDamage(float in_damage)
    {
        hit_damage = in_damage;
    }

    void SetDamageType(DamageType in_type)
    {
        damage_type = in_type;
    }

    void SetOverlapper(UShapeComponent* in_shape)
    {
        overlapper = in_shape;
    }

protected:
    // Called when the game starts or when spawned
    //virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override
    {
        Super::Tick(DeltaTime);
        TSet<AActor*> set;
        overlapper->GetOverlappingActors(set);
        if (enabled && tick_hit_enabled)
        {
            if (set.Num() > 0 && HasAuthority())
            {
                for (auto& act : set)
                {
                    int32 act_id = act->GetUniqueID();
                    if (!hit.Find(act) && act != GetOwner())
                    {
                        {
                            auto health_comp = Cast<IHealthInterface>(act);
                            if (health_comp)
                            {
                                DamageInstance shock_damage;
                                shock_damage.value = hit_damage;
                                shock_damage.damage_type = damage_type;
                                shock_damage.is_attack = false;
                                health_comp->ApplyDamage(shock_damage, GetOwner());
                                hit.Add(act);// (act->GetUniqueID());
                                UE_LOG(LogTemp, Warning, TEXT("SHOCK HIT CHARACTER: %f, %d"), health_comp->GetHealth(), act);
                            }
                            auto status_manager = Cast<UStatusManager>(act->GetComponentByClass(UStatusManager::StaticClass()));
                            if (status_manager)
                            {
                                auto status_effect = NewObject<UStatusEffect>(GetWorld(), status_effect_class);
                                status_manager->AddStatus(status_effect);
                            }
                        }
                    }
                }
            }
        }
        

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
	float DefaultCooldown;
    UPROPERTY( Replicated )
	float ManaCost;
	float CastRange;
    UPROPERTY(Replicated)
    float Cooldown;
    UPROPERTY(Replicated)
    float CurrentCooldown;
	class UDecalComponent* TargetingDecal;
    UPROPERTY()
    int32 max_level;
    UPROPERTY(Replicated)
    int32 current_level;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UAbility, ManaCost);
        DOREPLIFETIME(UAbility, Cooldown);
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
        return Cooldown;
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
        return ManaCost;
    }
	virtual void OnActivation() { UE_LOG(LogTemp, Warning, TEXT("Non-targeted activation")); }
	virtual void OnUnitActivation(AActor* target) { UE_LOG(LogTemp, Warning, TEXT("Unit-targeted activation")); }
	virtual void OnHit(DamageInstance& damage, AActor* unit) { UE_LOG(LogTemp, Warning, TEXT("OnHit activation")); }
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
};

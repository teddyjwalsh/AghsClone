// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GameModeInfo.generated.h"


USTRUCT(BlueprintType)
struct FCharacterSpec
{
	GENERATED_BODY()

		static TMap<FString, UTexture2D*> textures;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* MyTex;
	UPROPERTY(BlueprintReadWrite)
	UClass* CharClass;

	void SetTexture(FString to_load)
	{
		if (!textures.Find(to_load))
		{
			//ConstructorHelpers::FObjectFinder<UTexture2D> ScreenMat(*FString::Printf(TEXT("Texture2D'/Game/Textures/%s.%s'"), *to_load, *to_load));
			auto item_tex = Cast<UTexture2D>(
				StaticLoadObject(UTexture2D::StaticClass(),
					NULL,
					*FString::Printf(TEXT("/Game/Textures/%s.%s"),
						*to_load,
						*to_load)));
			textures.Add(to_load, item_tex);
		}
		MyTex = textures[to_load];
	}
};

UCLASS()
class AGHSCLONE_API AGameModeInfo : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TArray<FCharacterSpec> character_list;
	
public:	
	// Sets default values for this actor's properties
	AGameModeInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<FCharacterSpec>& GetCharacterList()
	{
		return character_list;
	}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AGameModeInfo, character_list);
	}

};

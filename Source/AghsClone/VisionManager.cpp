// Fill out your copyright notice in the Description page of Project Settings.

#include "VisionManager.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NetDriver.h"
#include "Engine/NetworkObjectList.h"


#include "VisionInterface.h"
#include "AghsClonePlayerController.h"

// Sets default values
AVisionManager::AVisionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	vision_bounds = CreateDefaultSubobject<USphereComponent>(TEXT("vision_bounds"));
	vision_bounds->SetAbsolute(true, true, true);

	//vision_positions = CreateDefaultSubobject<UMaterialParameterCollection>(TEXT("vision_positions"));
	vision_update_timer = 0.0;
	SetReplicates(false);
	vision_bounds->SetCollisionProfileName(TEXT("Vision"));

}

// Called when the game starts or when spawned
void AVisionManager::BeginPlay()
{
	Super::BeginPlay();
	first_iteration = true;
}

void AVisionManager::create_client_fow(AActor* act, const TArray<AActor*>& field_actors)
{
#if WITH_EDITOR
	ENetMode netMode = GetWorld()->GetNetMode();
	ENetRole netRole = GetLocalRole();
	ENetRole netRoleRemote = GetRemoteRole();
#endif

	FCollisionQueryParams ignore_me;
	//ignore_me.AddIgnoredActor(act);
	ignore_me.AddIgnoredActors(field_actors);

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;

	if (GetWorld()->IsClient())
	{
		printf("IS CLIENT WHEN SHOULD BE CLIENT!!!!");
	}

	int max_vision_rays = 100;
	Vertices.Add(vision_bounds->GetComponentLocation());
	for (int i = 0; i < max_vision_rays; ++i)
	{
		FHitResult out_hit;
		auto actor_loc = act->GetActorLocation();
		float angle = i * 2 * PI / (max_vision_rays - 2);
		FVector out_vec(cos(angle), sin(angle), 0);

		GetWorld()->LineTraceSingleByChannel(out_hit,
			vision_bounds->GetComponentLocation(),
			vision_bounds->GetComponentLocation() + out_vec * vision_bounds->GetScaledSphereRadius(),
			ECollisionChannel::ECC_WorldDynamic,
			ignore_me);

		if (out_hit.bBlockingHit)
		{
			Vertices.Add(vision_bounds->GetComponentLocation() + out_vec * (out_hit.Distance + 10));
			if (i != 0)
			{
				Triangles.Add(0);
				Triangles.Add(i);
				Triangles.Add(i - 1);
				Normals.Add(FVector(0, 0, 1));
			}
		}
		else
		{
			Vertices.Add(vision_bounds->GetComponentLocation() + out_vec * vision_bounds->GetScaledSphereRadius());
			if (i != 0)
			{
				Triangles.Add(0);
				Triangles.Add(i);
				Triangles.Add(i - 1);
				Normals.Add(FVector(0, 0, 1));
			}
		}
	}

	if (vision_meshes.find(act) == vision_meshes.end())
	{
		vision_meshes[act] = GetWorld()->SpawnActor<AFogOfWarMesh>();
	}
	auto aghs_char = Cast<AAghsCloneCharacter>(act);
	if (aghs_char)
	{
		vision_meshes[act]->SetVisionMesh(Vertices, Triangles, Normals);
		aghs_char->SetVisionMesh(vision_meshes[act]);
	}
}

// Called every frame
void AVisionManager::Tick(float DeltaTime)
{
	//if (!GetWorld()->IsServer())
	if (!HasAuthority())
	{
		return;
	}
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	ENetMode netMode = GetWorld()->GetNetMode();
	ENetRole netRole = GetLocalRole();
	ENetRole netRoleRemote = GetRemoteRole();
#endif

	TArray<AActor*> vision_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UVisionInterface::StaticClass(), vision_actors);
	TArray<AActor*> field_actors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UFieldActorInterface::StaticClass(), field_actors);
	auto old_sets = team_vision_sets;
	if (first_iteration)
	{
		for (auto& va : vision_actors)
		{
			auto vac = Cast<AAghsCloneCharacter>(va);
			if (vac)
			{
				old_sets[vac->GetTeam()] = std::set<AActor*>();
			}
		}
		for (auto& va : field_actors)
		{
			for (auto& team : old_sets)
			{
				team.second.insert(va);
			}
		}
		first_iteration = false;
	}
	for (auto& k : team_vision_sets)
	{
		team_vision_sets[k.first].clear();
	}
	
	for (auto& act : vision_actors)
	{
		auto aghs_unit = Cast<AAghsCloneCharacter>(act);
		int32 vision_team = aghs_unit->GetTeam();
		IVisionInterface* vision_interface = Cast<IVisionInterface>(act);
		vision_bounds->SetWorldLocation(act->GetActorLocation());
		vision_bounds->SetSphereRadius(vision_interface->GetVisionRadius());
		TSet<AActor*> near_chars;
		vision_bounds->GetOverlappingActors(near_chars);//, AAghsCloneCharacter::StaticClass());
		FCollisionQueryParams ignore_me;
		//ignore_me.AddIgnoredActor(act);
		ignore_me.AddIgnoredActors(field_actors);

		for (auto& near_actor : near_chars)
		{
			auto near_aghs_unit = Cast<AAghsCloneCharacter>(near_actor);
            if (near_aghs_unit)
            {
                if (act == near_actor || near_aghs_unit->GetTeam() == vision_team)
                {
                    team_vision_sets[vision_team].insert(near_actor);
                    continue;
                }
            }
			FHitResult out_hit;
			FVector distance_vector = near_actor->GetActorLocation() - act->GetActorLocation();
			FVector shoot_vector = distance_vector;
			shoot_vector.Normalize();

			GetWorld()->LineTraceSingleByChannel(out_hit, 
				vision_bounds->GetComponentLocation(), 
				vision_bounds->GetComponentLocation() + shoot_vector * vision_bounds->GetScaledSphereRadius(), 
				ECollisionChannel::ECC_WorldDynamic,
				ignore_me);
			float actor_distance = (act->GetActorLocation() - near_actor->GetActorLocation()).Size();
			if (out_hit.Distance > actor_distance || !out_hit.bBlockingHit)
			{
				team_vision_sets[vision_team].insert(near_actor);
				//act->Relev
			}
			else
			{
				//team_vision_sets[vision_team].erase(near_actor);
			}
		}
		/*
		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;

		int max_vision_rays = 100;
		Vertices.Add(vision_bounds->GetComponentLocation());
		for (int i = 0; i < max_vision_rays; ++i)
		{
			FHitResult out_hit;
			auto actor_loc = act->GetActorLocation();
			float angle = i * 2*PI / (max_vision_rays-2);
			FVector out_vec(cos(angle), sin(angle), 0);

			GetWorld()->LineTraceSingleByChannel(out_hit,
				vision_bounds->GetComponentLocation(),
				vision_bounds->GetComponentLocation() + out_vec * vision_bounds->GetScaledSphereRadius(),
				ECollisionChannel::ECC_WorldDynamic,
				ignore_me);

			if (out_hit.bBlockingHit)
			{
				Vertices.Add(vision_bounds->GetComponentLocation() + out_vec * (out_hit.Distance + 10));
				if (i != 0)
				{
					Triangles.Add(0);
					Triangles.Add(i);
					Triangles.Add(i-1);
					Normals.Add(FVector(0, 0, 1));
				}
			}
			else
			{
				Vertices.Add(vision_bounds->GetComponentLocation() + out_vec * vision_bounds->GetScaledSphereRadius());
				if (i != 0)
				{
					Triangles.Add(0);
					Triangles.Add(i);
					Triangles.Add(i - 1);
					Normals.Add(FVector(0, 0, 1));
				}
			}
		}
		
		
		if (vision_meshes.find(act) == vision_meshes.end())
		{
			//UProceduralMeshComponent* new_mesh = NewObject<UProceduralMeshComponent>(act, UProceduralMeshComponent::StaticClass());
			//act->AddOwnedComponent(new_mesh);
			//vision_meshes[act] = new_mesh;
			//vision_meshes[act]->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
			auto aghs_char = Cast<AAghsCloneCharacter>(act);
			if (aghs_char)
			{
				aghs_char->SetVisionMesh(Vertices, Triangles, Normals);
			}
		}

		*/
		if (!HasAuthority())
		{
			printf("IS CLIENT WHEN SHOULD BE SERVER");
		}
		if (vision_meshes.find(act) == vision_meshes.end())
		{
			vision_meshes[act] = GetWorld()->SpawnActor<AFogOfWarMesh>();
			auto aghs_char = Cast<AAghsCloneCharacter>(act);
			if (aghs_char)
			{
				//vision_meshes[act]->create_client_fow(act, vision_bounds, field_actors);
				aghs_char->SetVisionMesh(vision_meshes[act]);
			}
			vision_meshes[act]->SetVisionActor(act);
			
		}
		
		//vision_meshes[act]->
	}
	auto pit = GetWorld()->GetPlayerControllerIterator();
	int count = 0;
	bool new_controller = false;
	for (pit; pit; ++pit)
	{
		auto pc = Cast<AAghsClonePlayerController>(pit->Get());
		if (player_controllers.find(pc) == player_controllers.end())
		{
			new_controller = true;
		}
		if (pc)
		{
			/*
			FVector cam_loc;
			FRotator cam_rot;
			pc->PlayerCameraManager->GetCameraViewPoint(cam_loc, cam_rot);
			pc->PlayerCameraManager->Camera
			*/
			bool do_update = false;
			if (vision_update_timer > 2)
			{
				do_update = true;
				vision_update_timer = 0;
			}
			for (auto& act : field_actors)
			{
				bool new_actor = false;
				auto find_act = team_vision_sets[pc->GetTeam()].find(act);
				auto find_act_old = old_sets[pc->GetTeam()].find(act);
				if (field_actor_record.Find(act) == INDEX_NONE)
				{
					new_actor = true;
				}
				//if (GetGameTimeSinceCreation() > 2.0)
				{
					if (find_act == team_vision_sets[pc->GetTeam()].end() &&
						(find_act_old != old_sets[pc->GetTeam()].end() || new_controller))
					{
						pc->SetLocalActorVisibility(act, false);
					}
					else if (find_act != team_vision_sets[pc->GetTeam()].end() &&
						(find_act_old == old_sets[pc->GetTeam()].end() || new_controller))
					{
						pc->SetLocalActorVisibility(act, true);
					}
				}
				if (do_update)
				{
					if (find_act == team_vision_sets[pc->GetTeam()].end())
					{
						pc->SetLocalActorVisibility(act, false);
					}
					else
					{
						pc->SetLocalActorVisibility(act, true);
					}
					
				}
			}
		}
		player_controllers.insert(pc);
	}
	field_actor_record = field_actors;
	vision_update_timer += DeltaTime;
	return;
}

std::set<AActor*>& AVisionManager::get_team_visible(int32 in_team)
{
	return team_vision_sets[in_team];
}
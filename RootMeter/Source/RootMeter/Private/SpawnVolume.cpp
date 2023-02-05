// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "SpawnBlockingVolume.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/TransformCalculus3D.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	TreeClass = nullptr;
	ObstacleClass = nullptr;
	CurrentlySpawnedTrees = TArray<AActor*>();
	CurrentlySpawnedObstacles = TArray<AActor*>();
	SpawnedActorLocations = TArray<FVector>();
	NumOfTreesToSpawn = 0;
	TreeAvoidanceRadius = 0.0f;
	ObstacleAvoidanceRadius = 0.0f;
	EdgeAvoidanceMargin = 0.0f;
	bAreTreesSpawned = false;
	bDebugDraw = false;
	bSpawnInGame = false;
	ObstacleMaxScale = FVector::ZeroVector;
	bSpawnObstacles = true;
	ClearIgnoreClass = nullptr;
}

void ASpawnVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(bDebugDraw)
	{
		DebugDraw();
	}
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if(bSpawnInGame)
	{
		Spawn();
	}
}

void ASpawnVolume::BeginDestroy()
{
	Clear();

	bAreTreesSpawned = false;

	Super::BeginDestroy();
}

void ASpawnVolume::Spawn()
{
	Clear();

	const FRandomStream RandomStream(Seed);
	SpawnTrees(RandomStream);
	
	if(bAreTreesSpawned && bSpawnObstacles)
	{
		SpawnObstacles();
	}
}

void ASpawnVolume::SpawnTrees(FRandomStream RandomStream)
{
	UWorld* World = GetWorld();
	if(!IsValid(World) || !IsValid(TreeClass))
	{
		return;
	}

	bAreTreesSpawned = false;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const FBoxSphereBounds VolumeBounds = GetBounds();
	// Get the bounds of the map
	FVector MapMin = FVector(-VolumeBounds.BoxExtent.X, -VolumeBounds.BoxExtent.Y, 0.0f);
	FVector MapMax = FVector(VolumeBounds.BoxExtent.X, VolumeBounds.BoxExtent.Y, 0.0f);

	// Calculate the adjusted bounds for the random spawning locations
	FVector AdjustedMin = MapMin + FVector(EdgeAvoidanceMargin, EdgeAvoidanceMargin, 0.0f);
	FVector AdjustedMax = MapMax - FVector(EdgeAvoidanceMargin, EdgeAvoidanceMargin, 0.0f);

	FBox SpawnBounds = FBox(AdjustedMin, AdjustedMax);
	
	for(int i = 0; i < NumOfTreesToSpawn; i++)
	{
		TArray<AActor*> OverlappingActors;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		int32 MaxAvoidanceAttempts = 1000;
		int32 AvoidanceAttempts = 0;

		float LocalAvoidanceRadius = TreeAvoidanceRadius;

		AActor* SpawnedActor = nullptr;
		FVector RandomLocation = FVector::ZeroVector;
		while (AvoidanceAttempts < MaxAvoidanceAttempts)
		{
			// Generate a random location within the bounds
			RandomLocation = FVector(RandomStream.FRandRange(SpawnBounds.Min.X, SpawnBounds.Max.X),
									 RandomStream.FRandRange(SpawnBounds.Min.Y, SpawnBounds.Max.Y),
									 RandomStream.FRandRange(SpawnBounds.Min.Z, SpawnBounds.Max.Z));

			bool bOverlap = false;
			// Use the OverlapActors function to check if any other actors exist within the specified radius
			bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, RandomLocation, LocalAvoidanceRadius, ObjectTypes, TreeClass, TArray<AActor*>(), OverlappingActors);
			// Use the OverlapActors function to check if we are attempting to spawn within a blocking volume
			bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, RandomLocation, LocalAvoidanceRadius, ObjectTypes, ASpawnBlockingVolume::StaticClass(), TArray<AActor*>(), OverlappingActors);

			if (!bOverlap)
			{
				// If no actors exist within the specified radius and position is not in blocking volume spawn the actor at that location
				FTransform SpawnTM = FTransform(FRotator::ZeroRotator, RandomLocation);
				SpawnedActor = World->SpawnActor<AActor>(TreeClass, SpawnTM, SpawnInfo);
				break;
			}

			AvoidanceAttempts++;

			if(AvoidanceAttempts == 1000 && !IsValid(SpawnedActor))
			{
				LocalAvoidanceRadius *= 0.9f;
				AvoidanceAttempts = 0;
			}
		}

		if(IsValid(SpawnedActor))
		{
			CurrentlySpawnedTrees.Add(SpawnedActor);
			SpawnedActorLocations.Add(RandomLocation);
		}
	}

	if(CurrentlySpawnedTrees.Num() > 0)
	{
		bAreTreesSpawned = true;
	}
}

void ASpawnVolume::SpawnObstacles()
{
	UWorld* World = GetWorld();
	if(!IsValid(World) || !IsValid(ObstacleClass))
	{
		return;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	int32 NumActors = CurrentlySpawnedTrees.Num();
	int32 MaxAvoidanceAttempts = 1000;
	for (int32 x = 0; x < NumActors; x++)
	{
		for (int32 y = x + 1; y < NumActors; y++)
		{
			const AActor* CurrentActor = CurrentlySpawnedTrees[x];
			const AActor* NextActor = CurrentlySpawnedTrees[y];

			FVector LineStart = CurrentActor->GetActorLocation();
			FVector LineEnd = NextActor->GetActorLocation();

			TArray<AActor*> OverlappingActors;
			int32 AvoidanceAttempts = 0;
			float LocalAvoidanceRadius = ObstacleAvoidanceRadius;

			AActor* SpawnedActor = nullptr;
			FVector SpawnPoint = FVector::ZeroVector;
			while (AvoidanceAttempts < MaxAvoidanceAttempts)
			{
				float RandomPercentage = FMath::RandRange(0.0f, 1.0f);
				SpawnPoint = LineStart + (LineEnd - LineStart) * RandomPercentage;

				FRotator SpawnRotator = FRotator::ZeroRotator;
				SpawnRotator.Yaw = FMath::RandRange(0.0f, 360.0f);

				float ObstacleMaxScaleX = ObstacleMaxScale.X;
				float ObstacleMaxScaleY = ObstacleMaxScale.Y;
				float ObstacleMaxScaleZ = ObstacleMaxScale.Z;

				float RandomScaleX = FMath::RandRange(1.0f, ObstacleMaxScaleX);
				float RandomScaleY = FMath::RandRange(1.0f, ObstacleMaxScaleY);
				float RandomScaleZ = FMath::RandRange(1.0f, ObstacleMaxScaleZ);

				FVector SpawnScale = FVector(RandomScaleX, RandomScaleY, RandomScaleZ);

				bool bOverlap = false;
				bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, SpawnPoint, LocalAvoidanceRadius, ObjectTypes, TreeClass, TArray<AActor*>(), OverlappingActors);
				bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, SpawnPoint, LocalAvoidanceRadius, ObjectTypes, ObstacleClass, TArray<AActor*>(), OverlappingActors);
				bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, SpawnPoint, LocalAvoidanceRadius, ObjectTypes, ASpawnBlockingVolume::StaticClass(), TArray<AActor*>(), OverlappingActors);

				if (!bOverlap)
				{
					FTransform SpawnTM = FTransform(SpawnRotator, SpawnPoint, SpawnScale);
					SpawnedActor = World->SpawnActor<AActor>(ObstacleClass,	SpawnTM, SpawnInfo); 

					UStaticMeshComponent* StaticMeshComponent = SpawnedActor->FindComponentByClass<UStaticMeshComponent>();
					if(IsValid(StaticMeshComponent))
					{
						StaticMeshComponent->SetRelativeScale3D(SpawnScale);
					}
					break;
				}

				AvoidanceAttempts++;

				if(AvoidanceAttempts == 1000 && !IsValid(SpawnedActor))
				{
					LocalAvoidanceRadius *= 0.9f;
					AvoidanceAttempts = 0;
				}
			}

			if(IsValid(SpawnedActor))
			{
				CurrentlySpawnedObstacles.Add(SpawnedActor);
			}
		}
	}
}

void ASpawnVolume::Clear()
{
	const UWorld* World = GetWorld();
	if(!IsValid(World))
	{
		return;
	}

	SpawnedActorLocations.Empty();

	if(CurrentlySpawnedTrees.Num() > 0)
	{
		for(AActor* Actor : CurrentlySpawnedTrees)
		{
			if(IsValid(Actor) && !Actor->IsA(ClearIgnoreClass))
			{
				Actor->Destroy();
			}
		}
		
		CurrentlySpawnedTrees.Empty();
	}

	if(CurrentlySpawnedObstacles.Num() > 0)
	{
		for(AActor* Actor : CurrentlySpawnedObstacles)
		{
			if(IsValid(Actor) && !Actor->IsA(ClearIgnoreClass))
			{
				Actor->Destroy();
			}
		}
		
		CurrentlySpawnedObstacles.Empty();
	}
	
	const FBoxSphereBounds Bounds = GetBounds();

	TArray<AActor*> PreviouslySpawnedActors;
	TArray<AActor*> PreviouslySpawnedObstacles;
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	const TArray<AActor*, FDefaultAllocator> IgnoreActors;
	
	UKismetSystemLibrary::BoxOverlapActors(World, Bounds.Origin, Bounds.BoxExtent, ObjectTypesArray, TreeClass, IgnoreActors, PreviouslySpawnedActors);
	UKismetSystemLibrary::BoxOverlapActors(World, Bounds.Origin, Bounds.BoxExtent, ObjectTypesArray, ObstacleClass, IgnoreActors, PreviouslySpawnedObstacles);
	
	if(PreviouslySpawnedActors.Num() > 0)
	{
		for(AActor* Actor : PreviouslySpawnedActors)
		{
			if(IsValid(Actor) && !Actor->IsA(ClearIgnoreClass))
			{
				Actor->Destroy();
			}
		}
		
		PreviouslySpawnedActors.Empty();
	}

	if(PreviouslySpawnedObstacles.Num() > 0)
	{
		for(AActor* Actor : PreviouslySpawnedObstacles)
		{
			if(IsValid(Actor) && !Actor->IsA(ClearIgnoreClass))
			{
				Actor->Destroy();
			}
		}
		
		PreviouslySpawnedObstacles.Empty();
	}
}

void ASpawnVolume::DebugDraw()
{
	const UWorld* World = GetWorld();
	if(!IsValid(World) || !bAreTreesSpawned)
	{
		return;
	}

	const FVector Offset = FVector(0.0f, 0.0f, 50.0f);
	constexpr float LineThickness = 16.0f;
	const int32 NumActors = SpawnedActorLocations.Num();
	if (NumActors > 0)
	{
		for (int32 x = 0; x < NumActors; x++)
		{
			for (int32 y = x + 1; y < NumActors; y++)
			{
				FVector CurrentPoint = SpawnedActorLocations[x] + Offset;
				FVector NextPoint = SpawnedActorLocations[y] + Offset;

				DrawDebugLine(World, CurrentPoint, NextPoint, FColor::Red, false, -1.f, 1, LineThickness);
			}
		}		
	}
}
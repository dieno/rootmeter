// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "SpawnBlockingVolume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/TransformCalculus3D.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	TreeClass = nullptr;
	ObstacleClass = nullptr;
	CurrentlySpawnedActor = TArray<AActor*>();
	NumOfTreesToSpawn = 0;
	AvoidanceRadius = 0.0f;
	EdgeAvoidanceMargin = 0.0f;
	bDebugDraw = false;
}

void ASpawnVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const UWorld* World = GetWorld();
	if(!IsValid(World))
	{
		return;
	}

	FVector Offset = FVector(0.0f, 0.0f, 50.0f);

	// if(CurrentlySpawnedActor.Num() >= 2)
	// {
	// 	AActor* FirstActor = CurrentlySpawnedActor[0];
	// 	AActor* SecondActor = CurrentlySpawnedActor[1];
	// 	DrawDebugLine(World, FirstActor->GetActorLocation() + Offset, SecondActor->GetActorLocation() + Offset, FColor::Red, false, -1.f, 0, 8.f);
	// }

	if(bDebugDraw)
	{
		// Get the number of actors in the array
		int32 NumActors = CurrentlySpawnedActor.Num();
		
		// Iterate through the array of actors
		for (int32 x = 0; x < NumActors; x++)
		{
			// Iterate through the array of actors
			for (int32 y = 0; y < NumActors; y++)
			{
				if(x == y)
				{
					continue;
				}

				AActor* CurrentActor = CurrentlySpawnedActor[x];
				AActor* NextActor = CurrentlySpawnedActor[y];

				// Draw a debug line between the current actor's location and the next actor's location
				DrawDebugLine(World, CurrentActor->GetActorLocation() + Offset, NextActor->GetActorLocation() + Offset, FColor::Red, false, -1.f, 1, 8.f);
			}
			// Get the current actor and the next actor in the array
			//AActor* NextActor = CurrentlySpawnedActor[(i + 1) % NumActors];
		}
	}
}

void ASpawnVolume::Spawn()
{
	UWorld* World = GetWorld();
	if(!IsValid(World) || !IsValid(TreeClass))
	{
		return;
	}

	Clear();

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

		float LocalAvoidanceRadius = AvoidanceRadius;

		AActor* SpawnedActor = nullptr;
		FVector RandomLocation = FVector::ZeroVector;
		while (AvoidanceAttempts < MaxAvoidanceAttempts)
		{
			// Generate a random location within the bounds
			RandomLocation = FMath::RandPointInBox(SpawnBounds);

			// Use the OverlapActors function to check if any other actors exist within the specified radius
			bool bOverlap = false;

			bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, RandomLocation, LocalAvoidanceRadius, ObjectTypes, TreeClass, TArray<AActor*>(), OverlappingActors);
			bOverlap |= UKismetSystemLibrary::SphereOverlapActors(World, RandomLocation, LocalAvoidanceRadius, ObjectTypes, ASpawnBlockingVolume::StaticClass(), TArray<AActor*>(), OverlappingActors);

			if (!bOverlap)
			{
				// If no actors exist within the specified radius, spawn the actor at that location
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
			CurrentlySpawnedActor.Add(SpawnedActor);
			SpawnedActorPositions.Add(RandomLocation);
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

	if(CurrentlySpawnedActor.Num() > 0)
	{
		for(AActor* Actor : CurrentlySpawnedActor)
		{
			if(IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		
		CurrentlySpawnedActor.Empty();
	}
	
	const FBoxSphereBounds Bounds = GetBounds();

	TArray<AActor*> PreviouslySpawnedActors;
	const TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	const TArray<AActor*, FDefaultAllocator> IgnoreActors;
	
	UKismetSystemLibrary::BoxOverlapActors(GetWorld(), Bounds.Origin, Bounds.BoxExtent, ObjectTypesArray, TreeClass, IgnoreActors, PreviouslySpawnedActors);
	
	if(PreviouslySpawnedActors.Num() > 0)
	{
		for(AActor* Actor : PreviouslySpawnedActors)
		{
			if(IsValid(Actor))
			{
				Actor->Destroy();
			}
		}
		
		PreviouslySpawnedActors.Empty();
	}
}
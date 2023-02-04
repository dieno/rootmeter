// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "SpawnVolume.generated.h"

/**
 * 
 */
UCLASS()
class ROOTMETER_API ASpawnVolume : public AVolume
{
	GENERATED_BODY()
	
public:
	ASpawnVolume();

	virtual void Tick(float DeltaSeconds) override;

protected:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SpawnVolume")
	void Spawn();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SpawnVolume")
	void Clear();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	int32 NumOfTreesToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	float AvoidanceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	float EdgeAvoidanceMargin;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	// int32 NumOfObstaclesToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	TSubclassOf<AActor> TreeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	TSubclassOf<AActor> ObstacleClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	bool bDebugDraw;
	
	UPROPERTY()
	TArray<AActor*> CurrentlySpawnedActor;

	UPROPERTY()
	TArray<FVector> SpawnedActorPositions;
	
};

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

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

protected:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SpawnVolume")
	void Spawn();
	
	UFUNCTION(BlueprintCallable, Category = "SpawnVolume")
	void SpawnTrees(FRandomStream RandomStream);

	UFUNCTION(BlueprintCallable, Category = "SpawnVolume")
	void SpawnObstacles();

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "SpawnVolume")
	void Clear();

	UFUNCTION()
	void DebugDraw();

	UFUNCTION(BlueprintCallable,Category="SpawnVolume")
	void SetSpawnSeed(const int32 InSeed) { Seed = InSeed; }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	int32 Seed = 12345;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	int32 NumOfTreesToSpawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	float TreeAvoidanceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	float ObstacleAvoidanceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	float EdgeAvoidanceMargin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	FVector ObstacleMaxScale;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	TSubclassOf<AActor> TreeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	TSubclassOf<AActor> ObstacleClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	bool bSpawnObstacles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	bool bSpawnInGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	TSubclassOf<AActor> ClearIgnoreClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	bool bDebugDraw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	bool bAreTreesSpawned;

	UPROPERTY()
	TArray<AActor*> CurrentlySpawnedTrees;

	UPROPERTY()
	TArray<AActor*> CurrentlySpawnedObstacles;

	UPROPERTY()
	TArray<FVector> SpawnedActorLocations;


};

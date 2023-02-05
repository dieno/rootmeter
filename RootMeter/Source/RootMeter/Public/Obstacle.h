// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class ROOTMETER_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnVolume")
	UStaticMeshComponent* StaticMesh;
};

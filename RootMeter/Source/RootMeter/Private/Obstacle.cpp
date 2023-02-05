// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"


AObstacle::AObstacle()
{
	USceneComponent* Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
}


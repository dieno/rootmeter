// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBase_FOG.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"

AGameModeBase_FOG::AGameModeBase_FOG()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameModeBase_FOG::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UWorld* World = GetWorld();
	if(!IsValid(World))
	{
		return;
	}

	if(IsValid(RenderTargetReveal))
	{
		UKismetRenderingLibrary::ClearRenderTarget2D(World, RenderTargetReveal, FLinearColor::Black);
	}
}
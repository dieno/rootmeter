// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBase_FOG.generated.h"

class UTextureRenderTarget2D;

/**
 * 
 */
UCLASS()
class ROOTMETER_API AGameModeBase_FOG : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameModeBase_FOG();

	virtual void Tick(float DeltaSeconds) override;
 
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameModeBase_FOG")
	UTextureRenderTarget2D* RenderTargetReveal;
};

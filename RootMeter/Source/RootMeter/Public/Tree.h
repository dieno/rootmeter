// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tree.generated.h"

UCLASS()
class ROOTMETER_API ATree : public AActor
{
	GENERATED_BODY()
	
public:	
	ATree();

	void SetFogEnabled(bool NewFogEnabled);
	bool GetFogEnabled() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tree")
	bool bIsFogEnabled;
	
};

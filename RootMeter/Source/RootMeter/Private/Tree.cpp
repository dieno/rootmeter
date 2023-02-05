// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"


ATree::ATree()
{
	bIsFogEnabled = true;
}

void ATree::SetFogEnabled(bool NewFogEnabled)
{
	bIsFogEnabled = NewFogEnabled;
}

bool ATree::GetFogEnabled() const
{
	return bIsFogEnabled;
}

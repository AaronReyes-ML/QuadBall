// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallPlayerController.h"
#include "QuadBallGameInstance.h"
#include "QuadBallCharacter.h"
#include "Yin.h"
#include "Engine.h"
#include <memory>


void AQuadBallPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

bool AQuadBallPlayerController::SpawnCharacter_Validate()
{
	return true;
}

void AQuadBallPlayerController::SpawnCharacter_Implementation()
{

}
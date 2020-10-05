// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QuadBallPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API AQuadBallPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnCharacter();
	
};

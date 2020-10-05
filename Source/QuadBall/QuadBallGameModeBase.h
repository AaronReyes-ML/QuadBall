// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuadBallGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API AQuadBallGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AQuadBallGameModeBase();
protected:
	virtual void BeginPlay() override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
};

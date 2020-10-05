// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallGameModeBase.h"
#include "QuadBallGameInstance.h"
#include "Yin.h"
#include "QuadBallPlayerController.h"
#include "Kismet/GameplayStatics.h"

AQuadBallGameModeBase::AQuadBallGameModeBase()
{
	DefaultPawnClass = AYin::StaticClass();
}

void AQuadBallGameModeBase::BeginPlay()
{
	
}

UClass* AQuadBallGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	UQuadBallGameInstance* gm = Cast<UQuadBallGameInstance>(GetGameInstance());
	
	if (gm->playerAClass) return gm->playerAClass;
	else
	{
		return DefaultPawnClass;
	}

}

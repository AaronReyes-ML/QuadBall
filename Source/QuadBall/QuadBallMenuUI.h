// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuadBallMenuPawn.h"
#include "QuadBallCharacter.h"
#include "QuadBallMenuUI.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallMenuUI : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayer1SkelMesh(class ASkeletalMeshActor* p);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayer2SkelMesh(class ASkeletalMeshActor* p);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayer1(class AQuadBallCharacter* p);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayer2(class AQuadBallCharacter* p);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanDispose1(bool in);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCanDispose2(bool in);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowMenuUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void StarGame();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Skip();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetVolumeSliders(float SFX, float MUSIC, float MASTER);
	
};

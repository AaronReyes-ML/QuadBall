// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QuadBallCharacter.h"
#include "Animation/AnimSequence.h"
#include "QuadBallCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Head();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RightKick(int special = 0);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LeftKick(int special = 0);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Knee();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandstandOn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HandstandOff();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BoostLeft();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BoostRight();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CancelBoost();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Serve(bool fast);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoInteractAnim(int anim);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CancelInteractAnim();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayWin(int severity);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayChargeParticle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CancelChargeParticle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayBallReset();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayBeginLevelMontage();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayMenuIdle();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayMenuCharacterSelect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NeedToRefresh(bool ref);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetSwimming(bool swim);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayMenuAnimSpawn();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayMenuAnimDestroy();
};

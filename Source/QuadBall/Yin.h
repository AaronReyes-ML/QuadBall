// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadBallCharacter.h"
#include "Yin.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API AYin : public AQuadBallCharacter
{
	GENERATED_BODY()

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DoSpecialKick2(float dTime) override;
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_CommandBallSpecialMove(float dTime) override;

	class UParticleSystem* specialMove2Part1Sys;
	class UParticleSystem* specialMove2Part2Sys;
	float timeInSpecialMove2 = 0;
	int specialMove2Phase = 1;
	float specialMove2MaxTime = 2.5f;
	float specialMovePhase2Time = .25f;
	float specialMovePhase1Time = 1.f;
	
public:
	AYin();

};

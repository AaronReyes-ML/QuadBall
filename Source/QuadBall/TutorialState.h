// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class QUADBALL_API TutorialState
{

public:
	bool neverActive = false;
	bool canScorePoints = false;
	bool canFreeReset = false;
	bool canReset = true;
	bool doSpecialReset = false;
	bool alwaysInBounds = false;
	bool canSpecialDrive = true;
	bool unlimitSpecialDrive = false;
	bool alwaysUnclean = false;
	bool alwaysOOB = false;
	int forceBounceCount = -1;
	int forcePlayerResponsibility = -1;
	int forceStartSide = -1;
	FVector specialResetVector = FVector::ZeroVector;

	TutorialState();
	TutorialState(bool neverActive, bool canScorePoints, bool canReset, bool doSpecialReset, bool alwaysInBounds,
		bool canSpecialDrive, bool unlimitSpecialDrive, bool alwaysUnclean, bool alwaysOOB, int forceBounceCount,
		int forcePlayerResponsibility, int forceStartSide, FVector specialResetVector) : neverActive(neverActive), canScorePoints(canScorePoints), canReset(canReset),
		doSpecialReset(doSpecialReset), alwaysInBounds(alwaysInBounds), canSpecialDrive(canSpecialDrive), unlimitSpecialDrive(unlimitSpecialDrive),
		alwaysUnclean(alwaysUnclean), alwaysOOB(alwaysOOB), forceBounceCount(forceBounceCount), forcePlayerResponsibility(forcePlayerResponsibility),
		forceStartSide(forceStartSide), specialResetVector(specialResetVector) {};

	~TutorialState();
};

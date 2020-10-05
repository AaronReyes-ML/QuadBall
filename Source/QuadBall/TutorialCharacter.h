// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadBallCharacter.h"
#include "TutorialCharacter.generated.h"


USTRUCT()
struct FTutState
{
	GENERATED_BODY()

		bool canInteract = false;
	UPROPERTY(EditAnywhere)
		bool canDoSpecial = true;
	UPROPERTY(EditAnywhere)
		bool canBoost = true;
	UPROPERTY(EditAnywhere)
		bool canAckServe = true;
	UPROPERTY(EditAnywhere)
		bool neverActive = false;
	UPROPERTY(EditAnywhere)
		bool canScorePoints = false;
	UPROPERTY(EditAnywhere)
		bool canFreeReset = false;
	UPROPERTY(EditAnywhere)
		bool canReset = true;
	UPROPERTY(EditAnywhere)
		bool canKick = true;
	UPROPERTY(EditAnywhere)
		bool canServe = true;
	UPROPERTY(EditAnywhere)
		bool doSpecialReset = false;
	UPROPERTY(EditAnywhere)
		bool alwaysInBounds = false;
	UPROPERTY(EditAnywhere)
		bool canSpecialDrive = true;
	UPROPERTY(EditAnywhere)
		bool unlimitSpecialDrive = false;
	UPROPERTY(EditAnywhere)
		bool alwaysUnclean = false;
	UPROPERTY(EditAnywhere)
		bool alwaysOOB = false;
	UPROPERTY(EditAnywhere)
		int forceBounceCount = -1;
	UPROPERTY(EditAnywhere)
		int forceBallPlayStatus = -1;
	UPROPERTY(EditAnywhere)
		int forcePlayerResponsibility = -1;
	UPROPERTY(EditAnywhere)
		int forceStartSide = -1;
	UPROPERTY(EditAnywhere)
		FVector specialResetVector = FVector::ZeroVector;

	FTutState() {};
	FTutState(bool canInteract,bool canDoSpecial, bool canBoost, bool canAckServe, bool neverActive, bool canScorePoints,
		bool canFreeReset, bool canReset, bool canKick, bool canServe, bool doSpecialReset,
		bool alwaysInBounds, bool canSpecialDrive, bool unlimitSpecialDrive, bool alwaysUnclean,
		bool alwaysOOB, int forceBounceCount, int forceBallPlayStatus, int forcePlayerResponsibility,
		int forceStartSide, FVector specialResetVector) : canInteract(canInteract), canDoSpecial(canDoSpecial), canBoost(canBoost),
		canAckServe(canAckServe), neverActive(neverActive),canScorePoints(canScorePoints),canFreeReset(canFreeReset),
		canReset(canReset),canKick(canKick),canServe(canServe), doSpecialReset(doSpecialReset),
		alwaysInBounds(alwaysInBounds),canSpecialDrive(canSpecialDrive),unlimitSpecialDrive(unlimitSpecialDrive),
		alwaysUnclean(alwaysUnclean),alwaysOOB(alwaysOOB), forceBounceCount(forceBounceCount), forcePlayerResponsibility(forcePlayerResponsibility),
		forceStartSide(forceStartSide),specialResetVector(specialResetVector) {};
};


/**
 * 
 */
UCLASS()
class QUADBALL_API ATutorialCharacter : public AQuadBallCharacter
{
	GENERATED_BODY()
	
public:
	ATutorialCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TutNext();
	void TutPrev();
	void TutVis();
};

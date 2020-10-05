// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialBall.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "QuadBallCourt.h"
#include "Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "QuadBallCharacter.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"


/*
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
	int forceBallPlayStatus = -1;
	int forcePlayerResponsibility = -1;
	int forceStartSide = -1;
	FVector specialResetVector = FVector::ZeroVector;
	*/

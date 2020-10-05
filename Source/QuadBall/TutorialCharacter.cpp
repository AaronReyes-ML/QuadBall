// Fill out your copyright notice in the Description page of Project Settings.

#include "TutorialCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"
#include "QuadBallBall.h"
#include "QuadBallCourt.h"
#include "QuadBallUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "QuadBallCharacterAnimInstance.h"
#include "QuadBallGameInstance.h"
#include "WorldInteractable.h"
#include "InteractAnimInstance.h"
#include "QuadBallAutoCharacter.h"
#include "Engine/Texture2D.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/AmbientSound.h"
#include "Components/WidgetComponent.h"
#include "TutorialBall.h"


#pragma region construct

ATutorialCharacter::ATutorialCharacter()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> tutUI(TEXT("/Game/UI/Widgets/QuadBall_Tutorial_BP"));
	UIWidgetBase = tutUI.Class;
}

#pragma endregion

#pragma region setup

void ATutorialCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATutorialCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("TutNext", IE_Pressed, this, &ATutorialCharacter::TutNext);
	PlayerInputComponent->BindAction("TutPrev", IE_Pressed, this, &ATutorialCharacter::TutPrev);
	PlayerInputComponent->BindAction("TutVis", IE_Pressed, this, &ATutorialCharacter::TutVis);
}


#pragma endregion

#pragma region Tutorial

void ATutorialCharacter::TutNext()
{
	UIWidget->TutNext();
}

void ATutorialCharacter::TutPrev()
{
	UIWidget->TutPrev();
}

void ATutorialCharacter::TutVis()
{
	UIWidget->TutVis();
}

#pragma endregion


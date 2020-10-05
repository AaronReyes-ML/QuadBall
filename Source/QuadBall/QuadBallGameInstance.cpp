// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallGameInstance.h"
#include "ConstructorHelpers.h"
#include "Sound/SoundClass.h"

UQuadBallGameInstance::UQuadBallGameInstance()
{
	static ConstructorHelpers::FObjectFinder<USoundClass> sCl1(TEXT("SoundClass'/Game/Audio/SFX.SFX'"));
	SFX_SoundClass = Cast<USoundClass>(sCl1.Object);
	static ConstructorHelpers::FObjectFinder<USoundClass> sCl2(TEXT("SoundClass'/Game/Audio/New_MSTR.New_MSTR'"));
	MSTR_SoundClass = Cast<USoundClass>(sCl2.Object);
	static ConstructorHelpers::FObjectFinder<USoundClass> sCl3(TEXT("SoundClass'/Game/Audio/MUSIC.MUSIC'"));
	MUS_SoundClass = Cast<USoundClass>(sCl3.Object);
}

void UQuadBallGameInstance::SetMUSVol(float percent)
{
	MUS_SoundClass->Properties.Volume = FMath::Clamp(percent, .001f, 1.f);
	defaultMusicVol = MUS_SoundClass->Properties.Volume;
}

void UQuadBallGameInstance::SetSFXVol(float percent)
{
	SFX_SoundClass->Properties.Volume = FMath::Clamp(percent, .001f, 1.f);
	defaultSFXVol = SFX_SoundClass->Properties.Volume;
}

void UQuadBallGameInstance::SetMasterVol(float percent)
{
	MSTR_SoundClass->Properties.Volume = FMath::Clamp(percent, .001f, 1.f);
	defaultMasterVol = MSTR_SoundClass->Properties.Volume;
}



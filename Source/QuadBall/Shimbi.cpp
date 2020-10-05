// Fill out your copyright notice in the Description page of Project Settings.

#include "Shimbi.h"
#include "Components/SkeletalMeshComponent.h"
#include "QuadBallCharacterAnimInstance.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "QuadBallBall.h"
#include "QuadBallCourt.h"
#include "Engine.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

AShimbi::AShimbi()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> shimbiMesh(TEXT("/Game/ParagonShinbi/Characters/Heroes/Shinbi/Meshes/Shinbi"));
	playerSourceMesh = shimbiMesh.Object;
	GetMesh()->SetSkeletalMesh(playerSourceMesh);
	static ConstructorHelpers::FObjectFinder<USoundCue> shimbiVoice(TEXT("/Game/Characters/Shimbi/Voice/Voice_1/Shimbi_Paragon_Voice"));
	voiceCue = shimbiVoice.Object;
	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> shimbiAnim(TEXT("/Game/Characters/Shimbi/Shimbi_QuadBall_AnimBP1"));
	animInstanceSourceClass = shimbiAnim.Class;
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> shimbiMenuAnim(TEXT("/Game/Characters/Shimbi/Shimbi_Real_Menu_AnimBP"));
	menuAnimInstanceSourceClass = shimbiMenuAnim.Class;

	voiceComp->SetSound(voiceCue);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart1(TEXT("/Game/Particles/Default/Charge_Dark"));
	chargingEffect = shimbiPart1.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart2(TEXT("/Game/Particles/Default/Super_Active_Dark"));
	specialActiveEffect = shimbiPart2.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart3(TEXT("/Game/Particles/Default/Def_BallImpact_Particles"));
	specialBallHitEffect = shimbiPart3.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart4(TEXT("/Game/Particles/Default/Def_Burst_Particles"));
	specialPlayerBurst = shimbiPart4.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart5(TEXT("/Game/Particles/Default/Def_GameWin_Particles"));
	gameWinEffect = shimbiPart5.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> shimbiPart6(TEXT("/Game/Particles/Yin/Special2Phase1"));

	static ConstructorHelpers::FObjectFinder<UMaterial> shimbiSplash1(TEXT("/Game/UI/Character_Portraits/Shimbi_Splash"));
	characterPortrait = shimbiSplash1.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> shimbiSplash2(TEXT("/Game/UI/Character_Portraits/Shinbi_Splash"));
	characterPortraitTexture = shimbiSplash2.Object;
}



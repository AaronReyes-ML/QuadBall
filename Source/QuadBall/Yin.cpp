// Fill out your copyright notice in the Description page of Project Settings.

#include "Yin.h"
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


AYin::AYin()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> yinMesh(TEXT("/Game/ParagonYin/Characters/Heroes/Yin/Meshes/Yin"));
	playerSourceMesh = yinMesh.Object;
	GetMesh()->SetSkeletalMesh(playerSourceMesh);
	static ConstructorHelpers::FObjectFinder<USoundCue> yinVoice(TEXT("/Game/Characters/Yin/Voice/Voice1/Yin_Paragon_Voice"));
	voiceCue = yinVoice.Object;
	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> yinAnim(TEXT("/Game/Characters/Yin/Yin_QuadBall_AnimBP"));
	animInstanceSourceClass = yinAnim.Class;
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> yinMenuAnim(TEXT("/Game/Characters/Yin/Yin_Real_Menu_AnimBP"));
	menuAnimInstanceSourceClass = yinMenuAnim.Class;

	voiceComp->SetSound(voiceCue);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart1(TEXT("/Game/Particles/Default/Charge_Dark"));
	chargingEffect = yinPart1.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart2(TEXT("/Game/Particles/Default/Super_Active_Dark"));
	specialActiveEffect = yinPart2.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart3(TEXT("/Game/Particles/Default/Def_BallImpact_Particles"));
	specialBallHitEffect = yinPart3.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart4(TEXT("/Game/Particles/Default/Def_Burst_Particles"));
	specialPlayerBurst = yinPart4.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart5(TEXT("/Game/Particles/Default/Def_GameWin_Particles"));
	gameWinEffect = yinPart5.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart6(TEXT("/Game/Particles/Yin/Special2Phase1"));
	specialMove2Part1Sys = yinPart6.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> yinPart7(TEXT("/Game/Particles/Yin/Special2Phase2"));
	specialMove2Part2Sys = yinPart7.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> yinSplash1(TEXT("/Game/UI/Character_Portraits/Yin_Spash"));
	characterPortrait = yinSplash1.Object;
	static ConstructorHelpers::FObjectFinder<UTexture2D> yinSplash2(TEXT("/Game/UI/Character_Portraits/Yin"));
	characterPortraitTexture = yinSplash2.Object;

}

bool AYin::Server_DoSpecialKick2_Validate(float dTime) { return true; }

void AYin::Server_DoSpecialKick2_Implementation(float dTime)
{
	specialMove2Phase = 1;
	timeInSpecialMove2 = 0;
	activeBall->MULTICAST_PlaySpecialBallHit(specialMove2Part1Sys);
	FVector t = (activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt) - GetCapsuleComponent()->GetComponentLocation()) + FVector(0, 0, 1000);
	t.Normalize();
	activeBall->GetBallMesh()->SetPhysicsLinearVelocity(t * 1000);
}

bool AYin::Server_CommandBallSpecialMove_Validate(float dTime) { return true; }

void AYin::Server_CommandBallSpecialMove_Implementation(float dTime)
{
	if (isExecutingSpecialMove)
	{
		if (specialMoveToExecute == 2)
		{
			timeInSpecialMove2 += dTime;
			if (specialMove2Phase < 3)
			{
				if (timeInSpecialMove2 >= specialMovePhase1Time && specialMove2Phase == 1)
				{
					specialMove2Phase = 2;
					specialMovePhase2Time = FMath::FRandRange(.25, .75);
					activeBall->SetBallInteractivity(false);
					activeBall->GetBallMesh()->SetWorldLocation(activeCourt->GetMinimumSafeZLocation() + FVector(0, 0, 500));
				}
				else if (timeInSpecialMove2 >= specialMovePhase1Time + specialMovePhase2Time && specialMove2Phase == 2)
				{
					specialMove2Phase = 3;
					activeBall->MULTICAST_SetIsSpecialDriven(true);
					activeBall->SetBallInteractivity(true);
					
					Client_GetAim();

					FVector t = (activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt)) - activeBall->GetBallLocation();
					t.Normalize();
					activeBall->GetBallMesh()->SetPhysicsLinearVelocity(t * 1000);
					activeBall->MULTICAST_PlaySpecialBallHit(specialMove2Part2Sys);
				}
			}
			else
			{
				isExecutingSpecialMove = false;
			}
		}
		else
		{
			float specialTotalDist = specialDistToMid + specialDistToEnd;
			if (specialPhase1Dist < specialDistToMid || specialPhase2Dist < specialDistToEnd)
			{
				if (specialPhase1Dist < specialDistToMid)
				{
					float percent = ((specialPhase1Dist / specialDistToMid) / 2);
					float percent2 = ((specialPhase1Dist + (specialDistToMid / 100)) / specialDistToMid) / 2;

					FVector thisStep = specialCharLoc + (specialPhase1Dist * specialDirToMid);
					FVector nextStep = specialCharLoc + ((specialPhase1Dist + (specialDistToMid / 100)) * specialDirToMid);
					thisStep.Z = (characterSpecialCurve1->GetVectorValue(percent).Z) * specialMidPoint.Z;
					nextStep.Z = (characterSpecialCurve1->GetVectorValue(percent2).Z) * specialMidPoint.Z;

					FVector thisStepH = activeBall->GetBallLocation();
					FVector nextStepH = activeBall->GetBallLocation();
					FVector ballRight = FVector::CrossProduct(specialDirToMid, FVector::UpVector);
					ballRight.Normalize();

					

					thisStepH = thisStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent).Y) * 200;
					nextStepH = nextStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent2).Y) * 200;

					specialVelocity = nextStep - thisStep;
					specialVelocity = specialVelocity + (nextStepH - thisStepH);
					specialVelocity.Normalize();


					activeBall->GetBallMesh()->SetPhysicsLinearVelocity(specialVelocity * specialVel);

					specialPhase1Dist = (activeBall->GetBallLocation() - specialCharLoc).Size();
				}
				else
				{
					float percent = ((specialPhase2Dist / specialDistToEnd) / 2);
					float percent2 = ((specialPhase2Dist + (specialDistToEnd / 100)) / specialDistToEnd) / 2;

					if (percent2 < .5)
					{
						FVector thisStep = specialMidPoint + (specialPhase2Dist * specialDirToEnd);
						FVector nextStep = specialMidPoint + ((specialPhase2Dist + (specialDistToEnd / 100)) * specialDirToEnd);
						thisStep.Z = characterSpecialCurve1->GetVectorValue(.5 + percent).Z * specialMidPoint.Z;
						nextStep.Z = characterSpecialCurve1->GetVectorValue(.5 + percent2).Z * specialMidPoint.Z;

						FVector thisStepH = activeBall->GetBallLocation();
						FVector nextStepH = activeBall->GetBallLocation();
						FVector ballRight = FVector::CrossProduct(specialDirToEnd, FVector::UpVector);
						ballRight.Normalize();

						thisStepH = thisStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent).Y) * 200;
						nextStepH = nextStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent2).Y) * 200;

						specialVelocity = nextStep - thisStep;
						specialVelocity = specialVelocity + (nextStepH - thisStepH);
					}
					else
					{
						specialVelocity = activeBall->GetBallLocation() + FVector(0, 0, -1) * 100 - activeBall->GetBallLocation();
					}

					specialVelocity.Normalize();
					activeBall->GetBallMesh()->SetPhysicsLinearVelocity(specialVelocity * specialVel);

					specialPhase2Dist = (specialMidPoint - activeBall->GetBallLocation()).Size();
				}
			}
			else
			{
				isExecutingSpecialMove = false;
			}
		}
	}
}
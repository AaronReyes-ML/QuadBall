// Fill out your copyright notice in the Description page of Project Settings.

#include "AutoQuadBallCharacter.h"
#include "QuadBallCharacter.h"
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
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Sound/AmbientSound.h"
#include "Components/WidgetComponent.h"
#include "UnrealNetwork.h"

// General Constructor for the Automatically Controlled Character
AAutoQuadBallCharacter::AAutoQuadBallCharacter()
{
	static ConstructorHelpers::FObjectFinder<UCurveVector> curve99(TEXT("/Game/Curves/Def/Sppecial_Curve1"));
	characterSpecialCurve1 = curve99.Object;
}

// Initialize state for the automatically controlled character
void AAutoQuadBallCharacter::BeginPlay()
{
	// Skip the parent class because it initializes things that are not required for this class.
	// Super::Super called to instantiate standard character features
	Super::Super::BeginPlay();

	queryWaitTime = FMath::RandRange(1.f, queryWaitTimeMax);

	// This unit is always automatically controlled
	SelfIsAI = true;
	// Audio sources will play unless stopped
	effectComp->SetRelativeLocation(FVector::ZeroVector);
	effectComp->Stop();
	voiceComp->SetRelativeLocation(FVector::ZeroVector);
	voiceComp->Stop();
	// Hide the visual aim representation, it will always be hidden
	playerAimerComp->SetHiddenInGame(true);
	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	canControlMove = false;
	WorldUIWidget = Cast<UQuadBallUI>(inWorldUI->GetUserWidgetObject());
	UIWidget = Cast<UQuadBallUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	UIWidget->SetOwner(this);

	// Initialize other components
	chargeAmount = minCharge;

	aimX = .5;
	aimY = .5;

	if (GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
		characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
		if (!characterAnimInstance) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString("err"));

	}
	if (activeCourt)debugStatus = activeCourt->GetDebugStatus();

	if (autoPlayCharacter && activeCourt)
	{
		activeCourt->Server_QueForGame(this);
		return;
	}
	// AI units start the level "Idle"
	Client_InitPlayerForIdle();
}

void AAutoQuadBallCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAutoQuadBallCharacter, returned);
}

void AAutoQuadBallCharacter::HandleChallengeResponseTime(float DeltaTime)
{

	if (autoResponseTime > 0)
	{
		autoResponseTime -= DeltaTime;
	}
	else
	{
		challengeResponseTimerActive = false;
		autoResponseTime = autoResponseTimeMax;
		SendChallenge();
	}

}

void AAutoQuadBallCharacter::HandleMovement()
{
	FVector dist = activeBall->GetBallLocation() - GetMesh()->GetSocketLocation(FName("root"));
	// Handle movement
	if (activeCourt->GetBallSide(activeBall) == playerID)
	{
		if (!returned)
		{
			FVector ballTraject = activeBall->GetBallMesh()->GetComponentVelocity();
			ballTraject.Normalize();
			MoveCharacterToTarget(activeBall->GetBallLocation() + ballTraject * 150);
		}

		if (!handStand && hasKickedLeft && hasKickedRight)
		{
			Server_RequestHandstand();
		}

		if (dist.Size() < 200)
		{

			if (!handStand)
			{
				if (!hasKickedLeft && !hasKickedRight)
				{
					nextKickKey = FMath::RandRange(1, 2);
				}
				else if (!hasKickedLeft || !hasKickedRight)
				{
					if (!hasKickedLeft) nextKickKey = 1;
					else if (!hasKickedRight) nextKickKey = 2;
				}
			}
			else
			{
				if (!hasHandstandLeft && !hasHandstandRight)
				{
					nextKickKey = FMath::RandRange(3, 4);
				}
				else if (!hasHandstandLeft || !hasHandstandRight)
				{
					if (!hasKickedLeft) nextKickKey = 3;
					if (!hasKickedRight) nextKickKey = 4;
				}
			}
			

			isCharging = true;
			MULTICAST_PlayChargeParticles(nextKickKey);
			chargeMod = .6;
		}
		else
		{
			isCharging = false;
			MULTICAST_CancelChargeParticles();
			chargeMod = 1;
		}
	}
	else
	{
		if (isCharging)
		{
			isCharging = false;
			MULTICAST_CancelChargeParticles();
			chargeMod = 1;
		}
	}
}

void AAutoQuadBallCharacter::HandleKicking(float DeltaTime)
{
	FVector dist = activeBall->GetBallLocation() - GetMesh()->GetSocketLocation(FName("root"));
	// Handle kicking
	if (!returned && activeBall->GetPlayerBallResponsibility() == playerID && !isKicking)
	{
		if (kickCooldown > 0) kickCooldown -= DeltaTime;
		trainerStatus = 4;
		//Debug_PrintString(dist.Size());
		if (dist.Size() < returnDist && kickCooldown <= 0)
		{
			if (currentSpecialMove == 0)
			{
				isSpecialKicking = false;
			}
			else
			{
				isSpecialKicking = true;
			}

			if (nextKickKey == 1 || nextKickKey == 3) MULTICAST_KickLeft();
			else if (nextKickKey == 2 || nextKickKey == 4) MULTICAST_KickRight();

			kickCooldown = kickCooldownMax;

		}
	}
}

void AAutoQuadBallCharacter::WaitForPlayerServe()
{
	if ((startLocation - GetMesh()->GetComponentLocation()).Size() > 50)
	{
		MoveCharacterToTarget(startLocation);
		return;
	}

	// Wait for human player to indicate they are ready
	if (activeBall->GetPlayerBallResponsibility() == playerID)
	{
		trainerStatus = 1;
		if (activeBall->GetReadyToServe())
		{
			trainerStatus = 2;
			// Determine if the opposite player has indicated they are ready to serve
			bool acksrv = false;
			if (playerID == 0) acksrv = activeCourt->GetPlayerB()->GetAckServe();
			else acksrv = activeCourt->GetPlayerA()->GetAckServe();
			if (acksrv)
			{
				trainerStatus = 3;
				DoServe(false);
			}
		}
	}
}

void AAutoQuadBallCharacter::MoveToResetLocation()
{
	// Game is not active, move to start location
	if (playerID == 0) startLocation = activeCourt->GetPlayerASpawnLocation();
	else if (playerID == 1) startLocation = activeCourt->GetPlayerBSpawnLocation();
	//GEngine->AddOnScreenDebugMessage(103, 1, FColor::Green, FString("Moving to start"));
}

void AAutoQuadBallCharacter::HandleSpecialKick(float DeltaTime)
{
	if (currentATB == 2 && currentSpecialMove == 0 && !isExecutingSpecialMove)
	{
		if (specialMoveWaitTime < 0)
		{
			Server_SetSpecialMove(1);
			specialMoveWaitTime = FMath::FRandRange(1, 20);
		}
		else
		{
			specialMoveWaitTime -= DeltaTime;
		}
	}
}

void AAutoQuadBallCharacter::QueryAutoPlay()
{
	if (!HasAuthority() || challengeRequestActive) return;

	int randInt = FMath::RandRange((int32)1, (int32)3);
	if (randInt < 2) 
	{
		return;
	}

	isQueryingPlay = true;
	queryWaitTime = FMath::RandRange(1.f, queryWaitTimeMax);

	TArray<AActor*> allAisInWorld;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAutoQuadBallCharacter::StaticClass(), allAisInWorld);

	AAutoQuadBallCharacter* otherChar = nullptr;

	if (allAisInWorld.Num() > 1)
	{
		for (auto e : allAisInWorld)
		{
			otherChar = Cast<AAutoQuadBallCharacter>(e);
			if (otherChar == this || otherChar->GetIsPlaying() ||
				otherChar->GetChallengeRequestActive() || otherChar->GetIsQuerying()) continue;
			else
			{
				Server_SendChallengeRequest(otherChar);
				//Debug_PrintString(FString("Request sent from ") +  GetName() + " to " + otherChar->GetName(), -1, 50);
				break;
			}
		}
	}

	isQueryingPlay = false;
}

// Called every frame
void AAutoQuadBallCharacter::Tick(float DeltaTime)
{
	if (!HasAuthority()) return;

	if (gameMode != 1)
	{
		if (challengeResponseTimerActive) HandleChallengeResponseTime(DeltaTime);
		else
		{
			if (queryWaitTime < 0)
			{
				QueryAutoPlay();
			}
			else
			{
				queryWaitTime -= DeltaTime;
			}
		}

		return;
	}

	// Ball is in play
	if (activeBall->GetBallPlayStatus() != 0)
	{
		// Assess kicking logic
		if (isKicking)	AutoValidateKick();

		Server_SetATB(DeltaTime);
		HandleSpecialKick(DeltaTime);
		HandleMovement();
		HandleKicking(DeltaTime);

		if (isExecutingSpecialMove) Server_CommandBallSpecialMove(DeltaTime);

	}
	else
	{
		// Handle logic for AI only games
		if (bothAI)
		{
			// Point is over and ball has not been reset
			if (activeBall->GetNeedsToBeReset()) ResetFromPlayer();
			// If both units are AI and the opponent is responsible, this character must ack the serve
			if (activeBall->GetPlayerBallResponsibility() != playerID) ackServe = true;
		}

		MoveToResetLocation();
		WaitForPlayerServe();
	}
}

bool AAutoQuadBallCharacter::Towards()
{
	FVector charLoc = GetCapsuleComponent()->GetComponentLocation();
	FVector ballLoc = activeBall->GetBallLocation();
	FVector ballvel = activeBall->GetBallMesh()->GetPhysicsLinearVelocity();

	FVector ballToChar = charLoc - ballLoc;
	ballToChar.Normalize();

	FVector Result = ballvel * ballToChar;

	if (Result.X >= 0 && Result.Y >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AAutoQuadBallCharacter::AutoValidateKick()
{
	// Kick is good to proceed if ball is in range of kicking limb
	goodForKick = GetBallInInteractRange(kickingKey);
	if (goodForKick)
	{
		// Assign responsibility to current character if it is not currently set
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID, 0);
			activeBall->SetBallPlayStatus(TWO_BOUNCE_STATUS);
		}

		// New kicks during "2 bounce" status reset the total bounce count
		if (activeBall->GetBallPlayStatus() == TWO_BOUNCE_STATUS && kickingKey != 5)
		{
			activeBall->SetBallBounceCount(0);
		}

		// Kicking sets the ball to one bounce status, as long as it is not a head kick (body case handled by body block logic)
		if (kickingKey != 5) activeBall->SetBallPlayStatus(ONE_BOUNCE_STATUS);

		// Assess juggle, two same-limb juggles in a row is a loss of point
		if (activeBall->GetJuggleStatus())
		{
			if (lastJuggle != kickingKey || kickingKey == 5)
			{
				lastJuggle = kickingKey;
			}
			else
			{
				activeBall->SetBallPlayStatus(0);
				activeCourt->Server_AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2, "Double Juggle!");
			}
		}
		else
		{
			if ((kickingKey == 2 && hasKickedRight) || (kickingKey == 1 && hasKickedLeft) || (kickingKey == 3 && hasHandstandLeft) || (kickingKey == 4 && hasHandstandRight))
			{
				activeBall->SetBallPlayStatus(0);
				activeCourt->Server_AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2, "Double Kick!");
			}
			else
			{
				if (kickingKey == 1) hasKickedLeft = true;
				if (kickingKey == 2) hasKickedRight = true;
				//WorldUIWidget->ClearCard(kickingKey);
				lastJuggle = kickingKey;
				if (!lastKickChanceUsed)
				{
					lastKickChanceUsed = true;
					activeBall->SetBallBounceCount(0);
				}
			}
		}

		// If the Ball is being hit from the opposite side, it is not clean until it passes back over
		if (activeCourt->GetBallSide(activeBall) != playerID)
		{
			activeBall->Server_SetClean(false);
		}

		ReturnBall();
		activeBall->UpdateBallBounceCount(activeBall->GetBallBounceCount());
		activeBall->SetJuggleStatus(true);
		KickProbe(-1);
	}
}

bool AAutoQuadBallCharacter::MoveCharacterToTarget_Validate(FVector target) { return true; }
// Give input towards target from current position
void AAutoQuadBallCharacter::MoveCharacterToTarget_Implementation(FVector target)
{
	target.Z = 110;
	FVector direction = target - GetCapsuleComponent()->GetComponentLocation();
	direction.Normalize();

	AddMovementInput(direction, moveSpeedModifier * chargeMod, false);
}

void AAutoQuadBallCharacter::InformPlayerOfBounce()
{
	if (!HasAuthority()) return;
	returned = false;
}

// Attempt to return ball
void AAutoQuadBallCharacter::ReturnBall()
{
	if (!HasAuthority()) return;

	float aimX = FMath::FRandRange(-1.3, 1.3);
	float aimY = FMath::FRandRange(0, 1.3);
	float spinX = FMath::FRand();
	float spinY = FMath::FRand();
	myCourt = 1;
	float kickVelocity = 1000 * FMath::FRandRange(minF, maxF);

	if (isSpecialKicking)
	{
		Server_SpecialMoveInit();
		returned = true;
		return;
	}

	if (activeBall->GetIsSpecialDriven())
	{
		activeBall->MULTICAST_SetIsSpecialDriven(false);
	}

	if (kickingKey == 1) hasKickedLeft = true;
	if (kickingKey == 2) hasKickedRight = true;
	if (kickingKey == 3) hasHandstandLeft = true;
	if (kickingKey == 4) hasHandstandRight = true;

	FVector courtAimPos = activeCourt->GetPositionOnCourt(playerID, aimX, aimY, 1);
	FVector delta = courtAimPos - activeBall->GetBallLocation();

	bool firstTry = true;
	float temp = 0;
	float zCourt = 0;
	float x = delta.Size();
	float z = (activeBall->GetBallMesh()->GetComponentLocation().Z - zCourt);
	float v = kickVelocity;
	float g = 980;

	while (temp == 0)
	{
		if (!firstTry) v = v * 1.1;
		temp = (v * v * v * v) - (g * (g * (x * x) + 2 * z * (x * x)));
		temp = FMath::Sqrt(temp);
		firstTry = false;
		if (v >= kickVelocity * 10) temp = .5;
	}
	float angleRad = FMath::Atan2((v*v) + temp, g*x);
	delta.Normalize();

	FVector d = (delta * FVector(FMath::Cos(angleRad), FMath::Cos(angleRad), 0)) + FVector(0, 0, FMath::Sin(angleRad));
	d.Normalize();

	FVector dir = d * v;
	FVector spinDir = GetMesh()->GetForwardVector() * spinX + GetMesh()->GetRightVector() * spinY;

	activeBall->Server_DoBallAction(dir);
	activeBall->GetBallMesh()->SetPhysicsAngularVelocityInDegrees(spinDir * 2000);

	MULTICAST_DoHitVoice();
	effectComp->Play();

	isSpecialKicking = false;
	returned = true;
}

// Deprecated
void AAutoQuadBallCharacter::InitAIStatus(bool qBall)
{
	if (qBall)
	{
		characterAnimInstance->CancelInteractAnim();
		gameMode = 1;

		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->GravityScale = 1;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);

		//if (interactableObject) interactableObject->used = false;
	}
	else
	{
		if (!interactableObject) return;

		gameMode = 2;
		characterAnimInstance->DoInteractAnim(interactableObject->GetInteractType());

		GetCapsuleComponent()->SetEnableGravity(false);
		GetCharacterMovement()->GravityScale = 0;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);

		//interactableObject->used = true;
		FVector dir = interactableObject->GetExitPoint()->GetComponentLocation() - interactableObject->GetInteractPoint()->GetComponentLocation();
		dir.Z = 0;
		SetActorLocationAndRotation(interactableObject->GetInteractPoint()->GetComponentLocation(), dir.ToOrientationRotator());
	}
}

void AAutoQuadBallCharacter::MULTICAST_InteractAnim_Auto_Implementation(bool on, int interID)
{
	if (on)
	{
		GetCapsuleComponent()->SetEnableGravity(false);
		GetCharacterMovement()->GravityScale = 0;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);

		characterAnimInstance->DoInteractAnim(interID);
	}
	else
	{
		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->GravityScale = 1;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);

		characterAnimInstance->CancelInteractAnim();
	}
}

void AAutoQuadBallCharacter::Client_InitPlayerForQuadBall(class AQuadBallBall* ball, class AQuadBallCourt* court)
{
	activeCourt = court;
	activeBall = ball;
	
	bothAI = (activeCourt->GetPlayerA()->GetSelfIsAI() && activeCourt->GetPlayerB()->GetSelfIsAI());

	MULTICAST_InteractAnim_Auto(false);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);
	currentATB = 0;
	currentSpecialMove = 0;
	isSpecialKicking = false;
	gameMode = 1;
	canControlMove = true;

}

void AAutoQuadBallCharacter::Client_InitPlayerForIdle()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString("Derrived"));

	if (!interactableObject) return;

	gameMode = 2;
	MULTICAST_InteractAnim_Auto(true, interactableObject->GetInteractType());

	if (isCharging)
	{
		MULTICAST_CancelChargeParticles();
	}
	if (currentSpecialMove != 0)
	{
		MULTICAST_CancelSpecialActiveParticles();
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
	interactableObject->SetUsed(true);
	FVector dir = interactableObject->GetExitPoint()->GetComponentLocation() - interactableObject->GetInteractPoint()->GetComponentLocation();
	dir.Z = 0;
	SetActorLocationAndRotation(interactableObject->GetInteractPoint()->GetComponentLocation(), dir.ToOrientationRotator());
}

void AAutoQuadBallCharacter::Server_ResetAllKicks()
{
	if (!HasAuthority()) return;
	Super::Server_ResetAllKicks();
	if (handStand)
	{
		Server_RequestHandstand();
	}
	returned = false;
}
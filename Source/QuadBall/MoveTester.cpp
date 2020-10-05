// Fill out your copyright notice in the Description page of Project Settings.

#include "MoveTester.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"
#include "Curves/CurveFloat.h"


// Sets default values
AMoveTester::AMoveTester()
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootC = CreateDefaultSubobject<USceneComponent>("Root loc");
	RootComponent = rootC;

	restartPoint = CreateDefaultSubobject<USceneComponent>("Restart loc");
	restartPoint->SetupAttachment(RootComponent);

	playerCameraBoom = CreateDefaultSubobject<USpringArmComponent>("Arm");
	playerCameraBoom->SetupAttachment(RootComponent);
	playerCameraBoom->SetRelativeLocation(FVector::ZeroVector);
	playerCameraBoom->TargetArmLength = 1500;

	playerCamera = CreateDefaultSubobject<UCameraComponent>("cam");
	playerCamera->SetupAttachment(playerCameraBoom, USpringArmComponent::SocketName);
	playerCamera->SetRelativeLocation(FVector::ZeroVector);
	
	ball = CreateDefaultSubobject<UStaticMeshComponent>("ball");
	ball->SetupAttachment(RootComponent);
	ball->SetRelativeLocation(FVector::ZeroVector);
	ball->SetSimulatePhysics(false);

	playerAimerComp = CreateDefaultSubobject<UStaticMeshComponent>("Aimer");
	playerAimerComp->SetupAttachment(RootComponent);
	playerAimerComp->SetWorldLocation(FVector::ZeroVector);
	playerAimerComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	
}

// Called when the game starts or when spawned
void AMoveTester::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMoveTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, FString("Sim physics") + FString::FromInt(simPhysics)); FScreenMessageString* m0 = GEngine->ScreenMessages.Find(0); m0->TextScale.X = m0->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Cyan, FString("Total dist") + FString::FromInt(totalDist)); FScreenMessageString* m1 = GEngine->ScreenMessages.Find(1); m1->TextScale.X = m1->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Cyan, FString("Is jumping") + FString::FromInt(isJumping)); FScreenMessageString* m3 = GEngine->ScreenMessages.Find(3); m3->TextScale.X = m3->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Cyan, FString("Dist traveled") + FString::SanitizeFloat(distTraveled)); FScreenMessageString* m2 = GEngine->ScreenMessages.Find(2); m2->TextScale.X = m2->TextScale.Y = 1.0f;

	/*
	if (isJumping)
	{
		if (distTraveled != 0)
		{
			distTraveled = totalDist - (FVector(playerAimerComp->GetComponentLocation().X, playerAimerComp->GetComponentLocation().Y, 0) - FVector(ball->GetComponentLocation().X, ball->GetComponentLocation().Y, 0)).Size();
		}
		else
		{
			distTraveled = .05;
		}

		if (distTraveled < totalDist)
		{
			float percentOfCurveTraveled = distTraveled / totalDist;
			float oldPercentOfCurveTraveled = distTraveled / totalDist;

			float curveMin, curveMax;
			zCurve->GetTimeRange(curveMin, curveMax);

			vertMove.Z = ((zCurve->GetFloatValue(distTraveled) * totalDist) - (zCurve->GetFloatValue(oldDistTraveled) * totalDist));
			vertMove.Normalize();
			FVector forwardMove = aimDir * (distTraveled - oldDistTraveled);
			forwardMove.Normalize();
			oldDistTraveled = distTraveled;

			FVector tMove = vertMove + forwardMove;

			DrawDebugLine(GetWorld(), ball->GetComponentLocation(), ball->GetComponentLocation() + (tMove * 1000), FColor::Magenta, true, 100);

			ball->SetPhysicsLinearVelocity(tMove * (1000 * (totalDist / 1000)));
		}
		else
		{
			isJumping = false;
		}

	}
	*/

	
	if (isJumping)
	{
		timeInAir += DeltaTime;
		FVector forwardMove = aimDir;

		if (zCurve && xCurve)
		{
			float curveMin, curveMax;
			zCurve->GetTimeRange(curveMin, curveMax);
			if (timeInAir < curveMax)
			{
				vertMove.Z = (zCurve->GetFloatValue(timeInAir) - zCurve->GetFloatValue(oldTimeInAir));
				horizMove = rightDir * (xCurve->GetFloatValue(timeInAir) - xCurve->GetFloatValue(oldTimeInAir));
				oldTimeInAir = timeInAir;
			}
			else
			{
				isJumping = false;
			}
			
			totalMove = forwardMove + vertMove + horizMove;

			float velMod = (1000 * (totalDist / 1000));
			FMath::Clamp(velMod, 200.f, 2500.f);

			DrawDebugLine(GetWorld(), ball->GetComponentLocation(), ball->GetComponentLocation() + (totalMove * 1000), FColor::Magenta, true, 100);
			ball->SetPhysicsLinearVelocity(totalMove * velMod);

			//ball->SetWorldLocation((ball->GetComponentLocation() + totalMove));
		}
	}

	/*
		if (isJumping)
	{
		timeInAir += DeltaTime;
		FVector forwardMove = aimDir * (standardVel * totalDist * DeltaTime);

		if (zCurve && xCurve)
		{
			float curveMin, curveMax;
			zCurve->GetTimeRange(curveMin, curveMax);
			if (timeInAir < curveMax)
			{
				vertMove.Z = (zCurve->GetFloatValue(timeInAir) - zCurve->GetFloatValue(oldTimeInAir));
				horizMove = rightDir * (xCurve->GetFloatValue(timeInAir) - xCurve->GetFloatValue(oldTimeInAir));
				oldTimeInAir = timeInAir;
			}
			else
			{
				isJumping = false;
			}
			
			totalMove = forwardMove + vertMove + horizMove;

			ball->SetWorldLocation((ball->GetComponentLocation() + totalMove));
		}
	}*/
	

}

// Called to bind functionality to input
void AMoveTester::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("KickR", IE_Pressed, this, &AMoveTester::ChangeControlMode);
	PlayerInputComponent->BindAction("KickL", IE_Pressed, this, &AMoveTester::SetSimPhysics);
	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AMoveTester::Reset);
	PlayerInputComponent->BindAction("Head", IE_Pressed, this, &AMoveTester::Send);
	DECLARE_DELEGATE_OneParam(FSpecialIn, const float);
	PlayerInputComponent->BindAction<FSpecialIn>("ZoomIn", IE_Pressed, this, &AMoveTester::Zoom, 1.f);
	DECLARE_DELEGATE_OneParam(FSpecialOut, const float);
	PlayerInputComponent->BindAction<FSpecialOut>("ZoomOut", IE_Pressed, this, &AMoveTester::Zoom, -1.f);

	PlayerInputComponent->BindAxis("AimR", this, &AMoveTester::MoveR);
	PlayerInputComponent->BindAxis("AimU", this, &AMoveTester::MoveU);
}

void AMoveTester::Zoom(float inVal)
{
	if ((Controller != NULL) && (inVal != 0.0f))
	{
		if (inVal > 0)
		{
			if (playerCameraBoom->TargetArmLength < maxZoom)
			{
				playerCameraBoom->TargetArmLength += zoomAmount;
			}
		}
		else if (inVal < 0)
		{
			if (playerCameraBoom->TargetArmLength > minZoom)
			{
				playerCameraBoom->TargetArmLength -= zoomAmount;
			}
		}
	}
	FMath::Clamp(playerCameraBoom->TargetArmLength, minZoom, maxZoom);

}

void AMoveTester::MoveU(float inVal)
{
	if (controlMode == 0)
	{
		if ((Controller != NULL) && (inVal != 0.0f))
		{
			playerCamera->AddLocalRotation(FRotator(5 * inVal, 0, 0));
		}
	}
	else
	{
		playerAimerComp->AddLocalOffset(FVector(inVal * 10, 0, 0));
	}
}

void AMoveTester::MoveR(float inVal)
{
	if (controlMode == 0)
	{
		if ((Controller != NULL) && (inVal != 0.0f))
		{
			playerCameraBoom->AddWorldRotation(FRotator(0, 5 * inVal, 0));
		}
	}
	else
	{
		playerAimerComp->AddLocalOffset(FVector(0, inVal * 10, 0));
	}
}

void AMoveTester::Reset()
{
	isJumping = false;
	ball->SetWorldLocation(restartPoint->GetComponentLocation());
	playerAimerComp->SetWorldLocation(FVector::ZeroVector);
	ball->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void AMoveTester::SetSimPhysics()
{
	simPhysics = !simPhysics;
	ball->SetSimulatePhysics(simPhysics);
}

void AMoveTester::ChangeControlMode()
{
	if (controlMode == 0) controlMode = 1;
	else controlMode = 0;
}

void AMoveTester::Send()
{
	aimDir = FVector(playerAimerComp->GetComponentLocation().X, playerAimerComp->GetComponentLocation().Y, 0) - FVector(ball->GetComponentLocation().X, ball->GetComponentLocation().Y, 0);
	totalDist = aimDir.Size();
	aimDir.Normalize();

	//DrawDebugLine(GetWorld(), ball->GetComponentLocation(), ball->GetComponentLocation() + (aimDir * 1000), FColor::Red, true, 100);

	rightDir = FVector::CrossProduct(FVector::UpVector, aimDir);
	rightDir.Normalize();

	//DrawDebugLine(GetWorld(), ball->GetComponentLocation(), ball->GetComponentLocation() + (rightDir * 1000), FColor::Yellow, true, 100);

	isJumping = true;
	timeInAir = 0;
	oldTimeInAir = 0;
	distTraveled = 0;
	oldDistTraveled = 0;
	vertMove = FVector::ZeroVector;
	horizMove = FVector::ZeroVector;
	totalMove = FVector::ZeroVector;
}

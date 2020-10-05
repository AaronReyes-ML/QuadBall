// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallBall.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "QuadBallCourt.h"
#include "Engine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "QuadBallCharacter.h"
#include "QuadBallAutoCharacter.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

#pragma region construct
// Sets default values
AQuadBallBall::AQuadBallBall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ballRespawnerLocationA = CreateDefaultSubobject<USceneComponent>("Respawn Locale A");
	SetRootComponent(ballRespawnerLocationA);

	ballRespawnerLocationB = CreateDefaultSubobject<USceneComponent>("Respawn Locale B");
	ballRespawnerLocationB->SetupAttachment(ballRespawnerLocationA);
	ballRespawnerLocationB->SetRelativeLocation(FVector::ZeroVector);

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj0(TEXT("/Game/Meshes/Ball/QuadBall/QuadBall_Ball"));
	ballMesh->SetStaticMesh(obj0.Object);
	ballMesh->SetRelativeLocation(FVector::ZeroVector);
	ballMesh->SetWorldScale3D(FVector(1.3, 1.3, 1.3));

	ballMesh->SetNotifyRigidBodyCollision(true);
	ballMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	ballMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ballMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	ballMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Overlap);
	ballMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);

	ballIndicatorMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Ball position indicator");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> obj1(TEXT("/Game/Meshes/Props/Position_Indicator_Skel"));
	ballIndicatorMesh->SetSkeletalMesh(obj1.Object);
	ballIndicatorMesh->SetupAttachment(RootComponent);
	ballIndicatorMesh->SetRelativeLocation(FVector::ZeroVector);
	ballIndicatorMesh->SetHiddenInGame(true);

	ballHitAudioComponent = CreateDefaultSubobject<UAudioComponent>("Ball Hit Audio");
	ballHitAudioComponent->SetupAttachment(ballMesh);

	static ConstructorHelpers::FObjectFinder<USoundCue> cue1(TEXT("/Game/Audio/SFX/BallHit"));
	ballHitSoundCue = cue1.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> part1(TEXT("/Game/Particles/Default/Ball_Special"));
	specialDrivenSystem = part1.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> part2(TEXT("/Game/Particles/Default/Ball_Reset"));
	ballResetSystem = part2.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> part3(TEXT("/Game/Particles/Default/Def_Unclean"));
	ballUncleanSystem = part3.Object;

	ballHitAudioComponent->SetSound(ballHitSoundCue);

	bReplicates = true;
}

#pragma endregion

#pragma region begin
// Called when the game starts or when spawned
void AQuadBallBall::BeginPlay()
{
	ballMesh->SetMassOverrideInKg(FName(), 100.f);
	Super::BeginPlay();
	ballHitAudioComponent->Stop();
	ballMesh->SetSimulatePhysics(false);
	ballPlayStatus = 0;
	playerBallResponsibility = 0;

	if (court)
	{
		hasValidCourt = true;
	}

	ballMesh->OnComponentHit.AddDynamic(this, &AQuadBallBall::OnBallHit);
	ballMesh->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallBall::OnOverlapBeginClean);
}

#pragma endregion

void AQuadBallBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQuadBallBall, readyToServe);
	DOREPLIFETIME(AQuadBallBall, isServe);
	DOREPLIFETIME(AQuadBallBall, hasKickedServe);
	DOREPLIFETIME(AQuadBallBall, newServe);
	DOREPLIFETIME(AQuadBallBall, ballPlayStatus);
	DOREPLIFETIME(AQuadBallBall, juggle);
	DOREPLIFETIME(AQuadBallBall, isSpecialDriven);
	DOREPLIFETIME(AQuadBallBall, clean);
	DOREPLIFETIME(AQuadBallBall, currentBallSide);
	DOREPLIFETIME(AQuadBallBall, ballBounceCount);
	DOREPLIFETIME(AQuadBallBall, playerBallResponsibility);
	DOREPLIFETIME(AQuadBallBall, needsToBeReset);
	DOREPLIFETIME(AQuadBallBall, oBLocMarked);
	DOREPLIFETIME(AQuadBallBall, updateLoc);

}

// Called every frame
void AQuadBallBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick logic only required when quad ball game is active
	if (ballIndicatorMesh->bHiddenInGame) return;

	// Set ball indicator location at the floor under the ball's current location
	float zLoc = court->GetCourtFloor()->GetComponentLocation().Z + 25;
	ballIndicatorMesh->SetWorldLocation(FVector(ballMesh->GetComponentLocation().X, ballMesh->GetComponentLocation().Y, zLoc));
	ballIndicatorMesh->SetWorldRotation(FRotator::ZeroRotator);
	float scaleMult = maxScaleSize * (FMath::Abs(ballMesh->GetComponentLocation().Z - zLoc) / maxHight);
	// Adjust indicator scale based on ball's height from ground
	if (scaleMult > maxScaleSize) scaleMult = maxScaleSize;
	if (scaleMult < 1) scaleMult = 1;
	ballIndicatorMesh->SetRelativeScale3D(FVector(scaleMult, scaleMult, scaleMult));

	if (!HasAuthority()) return;
	if (doubleBounceRemain > 0)
	{
		doubleBounceRemain -= GetWorld()->GetDeltaSeconds();
	}
	if (doubleBounceRemain < 0) doubleBounceRemain = 0;

	if (updateLoc) MULTICAST_UpdateClientBallLoc(ballMesh->GetComponentLocation(), ballMesh->GetComponentRotation());
}

void AQuadBallBall::MULTICAST_UpdateClientBallLoc_Implementation(const FVector inLoc, const FRotator inRot)
{
	ballMesh->SetWorldLocation(inLoc);
	ballMesh->SetWorldRotation(inRot);
}

bool AQuadBallBall::Server_RequestBallReset_Validate() { return true; }

void AQuadBallBall::Server_RequestBallReset_Implementation()
{
	if (court->CheckEndGame()) return;
	court->CheckSwitchSide();
	court->MULTICAST_HidePoint();
	updateLoc = false;

	playerBallResponsibility = court->GetBallStartSide();
	currentBallSide = playerBallResponsibility;
	UpdateBallBounceCount(0);
	ballPlayStatus = 0;
	needsToBeReset = false;
	clean = false;
	juggle = false;
	readyToServe = true;
	MULTICAST_SetIsSpecialDriven(false);

	court->Server_ResetAllPlayerKicks();
	court->MULTICAST_CancelIndicateParticles();
	court->MULTICAST_UpdatePlayerResponsibilityDisplay(playerBallResponsibility);

	ClearOBLoc();

	ballMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	ballMesh->SetSimulatePhysics(false);

	MULTICAST_PlaySpecialBallHit(ballResetSystem);

	MULTICAST_ResetBallLocation(playerBallResponsibility);

	if (playerBallResponsibility == 0)
	{
		if (court->GetPlayerA())
		{
			ballMesh->AttachToComponent(court->GetPlayerA()->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), FName("BallServeSock"));
		}
	}
	else if (playerBallResponsibility == 1)
	{
		if (court->GetPlayerB())
		{
			ballMesh->AttachToComponent(court->GetPlayerB()->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), FName("BallServeSock"));
		}
	}
}

bool AQuadBallBall::MULTICAST_ResetBallLocation_Validate(int id) { return true; }

// Initiate ball reset process and reset ball critical values
void AQuadBallBall::MULTICAST_ResetBallLocation_Implementation(int id)
{
	if (id == 0)
	{
		if (court->GetPlayerA())
		{
			ballMesh->AttachToComponent(court->GetPlayerA()->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), FName("BallServeSock"));
		}
	}
	else if (id == 1)
	{
		if (court->GetPlayerB())
		{
			ballMesh->AttachToComponent(court->GetPlayerB()->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), FName("BallServeSock"));
		}
	}
}

bool AQuadBallBall::Server_RequestServe_Validate() { return true; }

void AQuadBallBall::Server_RequestServe_Implementation()
{
	SetBallPlayStatus(1);
	readyToServe = false;
	isServe = true;
	hasKickedServe = false;

	updateLoc = true;

	MULTICAST_ServeBall();
	ballMesh->SetSimulatePhysics(true);
	ballMesh->SetPhysicsLinearVelocity(FVector::UpVector * 500);
}

bool AQuadBallBall::MULTICAST_ServeBall_Validate() { return true; }

// Serve ball action 
void AQuadBallBall::MULTICAST_ServeBall_Implementation()
{
	ballMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

bool AQuadBallBall::Server_DoBallAction_Validate(const FVector& impuse) { return true; }

// Ball generic force in direction
void AQuadBallBall::Server_DoBallAction_Implementation(const FVector& impulse)
{
	if (isServe && hasKickedServe && ((playerBallResponsibility) % 2) != court->GetBallStartSide()) court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Double Kick Serve");

	ballMesh->SetPhysicsLinearVelocity(impulse, false);
	hasKickedServe = true;
}

void AQuadBallBall::MULTICAST_PlayBallHitEffect_Implementation(const float value)
{
	ballHitAudioComponent->SetFloatParameter(TYPE_PARAM, value);
	ballHitAudioComponent->Play();
}

// Mark out of bounds position
void AQuadBallBall::MULTICAST_MarkOBLoc_Implementation(FVector loc)
{
	oBLoc = loc;
	oBLocMarked = true;
	//DrawDebugLine(GetWorld(), oBLoc, oBLoc + FVector(0, 0, 100), FColor::Red, false, 25);
	court->MarkOBLocation(loc);
}

// Reset out of bounds position
void AQuadBallBall::ClearOBLoc()
{
	oBLocMarked = false;
}

#pragma region utilities

void AQuadBallBall::DebugResetBall()
{
	ballMesh->SetWorldLocation(court->GetMinimumSafeZLocation() + FVector(0, 0, 50));
}

// Get ball location in world space
FVector AQuadBallBall::GetBallLocation()
{
	return ballMesh->GetComponentLocation();
}

// Init ball side to invalid
int AQuadBallBall::GetNewBallSide()
{
	return -1;
}

// Determines if the ball is impacted within the court boundary
bool AQuadBallBall::ImpactInBounds(FVector impact)
{
	// Transform ball and extreme points to court space
	FVector thisLoc = court->GetCourtBasis().InverseTransformPosition(ballMesh->GetComponentLocation());

	// Determine if ball is inside the boundary box
	bool isInside = false;
	if (thisLoc.X < court->GetTopRight().X && thisLoc.X > court->GetBottomLeft().X && thisLoc.Y < court->GetTopRight().Y && thisLoc.Y > court->GetBottomLeft().Y) isInside = true;
	else isInside = false;

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString::FromInt(isInside));

	return isInside;
}

// Update the ball bounce count to passed in value
void AQuadBallBall::UpdateBallBounceCount(int c)
{
	ballBounceCount = c;
	int max = 0;
	if (ballPlayStatus == TWO_BOUNCE_STATUS) max = 2;
	else if (ballPlayStatus == ONE_BOUNCE_STATUS) max = 1;
	court->MULTICAST_UpdateBallBounceDisplay(ballBounceCount, max);
}

// Assigns responsibility to inVal, update bounce count
void AQuadBallBall::SetPlayerBallResponsibility(int inVal, int bounce, bool force)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString("Assing Response ") + FString::FromInt(inVal));
	UpdateBallBounceCount(bounce);

	if ((force || clean) && ballPlayStatus != 0 )
	{
		playerBallResponsibility = inVal;
		court->Server_ResetAllPlayerKicks();
		juggle = false;

		if (ballPlayStatus != 0)
		{
			ballPlayStatus = 1;
		}
	}
	else if (ballPlayStatus == 0)
	{
		playerBallResponsibility = inVal;
		court->Server_ResetAllPlayerKicks();
		juggle = false;
	}

	isServe = false;
	newServe = false;
	UpdateBallBounceCount(ballBounceCount);
	court->MULTICAST_UpdatePlayerResponsibilityDisplay(playerBallResponsibility);
}

// Set position indicator visibility
void AQuadBallBall::MULTICAST_SetPositionIndicatorVisibleStatus_Implementation(bool vis)
{

	vis ? ballIndicatorMesh->SetHiddenInGame(false) : ballIndicatorMesh->SetHiddenInGame(true);
}

// Set special driven status of ball
void AQuadBallBall::MULTICAST_SetIsSpecialDriven_Implementation(bool in)
{
	if (in)
	{
		isSpecialDriven = true;
		specialDrivenComponent = UGameplayStatics::SpawnEmitterAttached(specialDrivenSystem, ballMesh, FName(), FVector::ZeroVector, ballMesh->GetPhysicsLinearVelocity().ToOrientationRotator());
	}
	else
	{
		if (court->GetPlayerA()) court->GetPlayerA()->Server_SetIsExecSpecial(false);
		if (court->GetPlayerB()) court->GetPlayerB()->Server_SetIsExecSpecial(false);
		if (specialDrivenComponent)
		{
			specialDrivenComponent->DestroyComponent();
		}
		isSpecialDriven = false;
	}
}

void AQuadBallBall::MULTICAST_BallInteractivity_Implementation(bool set)
{
	ballMesh->SetHiddenInGame(!set);
	ballIndicatorMesh->SetHiddenInGame(!set);
}

void AQuadBallBall::SetBallInteractivity(bool set)
{
	ballMesh->SetSimulatePhysics(set);
	MULTICAST_BallInteractivity(set);
}

bool AQuadBallBall::Server_SetClean_Validate(bool in) { return true; }

void AQuadBallBall::Server_SetClean_Implementation(bool in)
{
	if (!HasAuthority()) return;
	clean = in;
	if (!clean)
	{
		MULTICAST_PlaySpecialBallHit(ballUncleanSystem);
	}
}

#pragma endregion

#pragma region Ball actions

void AQuadBallBall::BallSetPhysics_Implementation(bool on)
{

}

// DEPRICATED
bool AQuadBallBall::DoBallReset_Validate() { return true; }
void AQuadBallBall::DoBallReset_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Red, FString("IMPROPER CALL TO DOBALLRESET"));
}

// Deprecated
void AQuadBallBall::KickAtAngleWithForce(float rad, FVector dir, float mag)
{

}

bool AQuadBallBall::Server_BallHit_Validate(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { return true; }

void AQuadBallBall::Server_BallHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (doubleBounceRemain != 0 || !court->GetIsInSession()) return;
	// Increment Bounce Count

	if (court)
	{
		if (isSpecialDriven)
		{
			MULTICAST_SetIsSpecialDriven(false);
		}

		court->GetPlayerA()->InformPlayerOfBounce();
		court->GetPlayerB()->InformPlayerOfBounce();

		float sE_value = STANDARD_HIT;

		if (ballPlayStatus != NOT_PLAY_STATUS)
		{
			// Handle impacts in bounds of court
			if (OtherComp == court->GetCourtFloor() || (OtherComp != court->GetCourtNet() && ImpactInBounds(HitComp->GetComponentLocation())))
			{
				juggle = false;

				// A is responsible
				if (playerBallResponsibility == 0)
				{
					// Ball bounces on A side
					if (court->GetBallSide(this) == 0)
					{
						// Increase bounce count
						UpdateBallBounceCount(ballBounceCount + 1);
					}
					// Ball bounces on B side
					else if (court->GetBallSide(this) == 1)
					{
						// Transfer ball responsibility to B
						if (clean) SetPlayerBallResponsibility(1, 1);
						else UpdateBallBounceCount(ballBounceCount + 1);
					}
				}
				else if (playerBallResponsibility == 1)
				{
					if (court->GetBallSide(this) == 1)
					{
						UpdateBallBounceCount(ballBounceCount + 1);
					}
					else if (court->GetBallSide(this) == 0)
					{
						// Transfer ball responsibility to A
						if (clean) SetPlayerBallResponsibility(0, 1);
						else
						{
							UpdateBallBounceCount(ballBounceCount + 1);
						}
					}
				}
			}
			else if (OtherComp == court->GetCourtNet()) // Handle impats with net
			{
				sE_value = NET_HIT;
				if (isServe)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Net Hit!");
				}
			}
			else if (OtherComp == court->GetOB1() || OtherComp == court->GetOB2() || OtherComp == court->GetOB3() || OtherComp == court->GetOB4()) // Handle hits on boundary walls
			{
				if (isServe)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Serve Out of Bounds!");
				}
				else
				{
					//UpdateBallBounceCount(ballBounceCount + 1);
					//juggle = false;
				}
			}
			else if (OtherComp == court->GetCourtOBFloor() || (OtherComp != court->GetCourtNet() && !ImpactInBounds(HitComp->GetComponentLocation()))) // Handle out of bounds hits
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString("Hit OB floor"));
				//DrawDebugLine(GetWorld(), ballMesh->GetComponentLocation(), NormalImpulse, FColor::Red, true, 10);
				// Hit OB floor

				if (court->GetBallSide(this) != playerBallResponsibility)
				{
					MULTICAST_MarkOBLoc(ballMesh->GetComponentLocation());
					clean = false;
				}

				if (isServe)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Serve Out of Bounds!");
				}
				else
				{
					UpdateBallBounceCount(ballBounceCount + 1);
					juggle = false;
				}
			}

			if (ballPlayStatus == TWO_BOUNCE_STATUS)
			{
				if (isServe)
				{
					if (ballBounceCount > 1)
					{
						court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Serve Bounce Count Exceeded!");
					}
				}
				else if (ballBounceCount > 2)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Bounce Count Exceeded!");
				}
			}
			else if (ballPlayStatus == ONE_BOUNCE_STATUS)
			{
				if (isServe && hasKickedServe)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Ball Bounce After Serve!");
				}
				else if (ballBounceCount > 1)
				{
					court->Server_AwardPoint((playerBallResponsibility + 1) % 2, "Bounce Count Exceeded!");
				}
			}
		}

		MULTICAST_PlayBallHitEffect(sE_value);
		doubleBounceRemain = doubleBounceMax;
	}
}

// On ball collide with surface
void AQuadBallBall::OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HasAuthority())
	{
		Server_BallHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	}
}

bool AQuadBallBall::Server_OverlapClean_Validate(UPrimitiveComponent* OtherComp) { return true; }

void AQuadBallBall::Server_OverlapClean_Implementation(UPrimitiveComponent* OtherComp)
{
	//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Cyan, FString("Clean"));
	if (OtherComp == court->GetCleanSheet())
	{
		if (ballMesh->GetComponentLocation().Z >= court->GetMinimumSafeZLocation().Z && juggle == true)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Cyan, FString("True"));
			clean = true;
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 25, FColor::Cyan, FString("False"));
			clean = false;
		}
	}
}

// Handle ball pass over center net, renders ball as "clean" as long as it passes over the minimum z value for the net (prevents ball passing under net)
void AQuadBallBall::OnOverlapBeginClean(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	Server_OverlapClean(OtherComp);
}

#pragma endregion

#pragma region particles 
// Play ball special hit particles
void AQuadBallBall::MULTICAST_PlaySpecialBallHit_Implementation(UParticleSystem* inSys)
{

	ballHitComponent = UGameplayStatics::SpawnEmitterAttached(inSys, ballMesh);
	ballHitComponent->bAutoDestroy = true;
}

#pragma endregion

#pragma region settings
// Change sfx volume for ball components
void AQuadBallBall::ChangeSFXVolume(float percent)
{
	if (!HasAuthority()) return;
	ballHitAudioComponent->SetFloatParameter(SFX_VOL, percent);
}

// Change master volume for ball components
void AQuadBallBall::ChangeMasterVolume(float percent)
{
	if (!HasAuthority()) return;
	ballHitAudioComponent->SetFloatParameter(MAST_VOL, percent);
}

#pragma endregion


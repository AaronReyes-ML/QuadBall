// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallCourt.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ConstructorHelpers.h"
#include "QuadBallBall.h"
#include "Engine.h"
#include "QuadBallCharacter.h"
#include "Components/TextRenderComponent.h"
#include "QuadBallAutoCharacter.h"
#include "QuadBallGameInstance.h"
#include "RefAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "AutoQuadBallCharacter.h"
#include "UnrealNetwork.h"
#include "Sound/AmbientSound.h"

#pragma region construct

// Sets default values
AQuadBallCourt::AQuadBallCourt()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	primaryRefereeMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Score Ref");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> obj01010(TEXT("/Game/ParagonYin/Characters/Heroes/Yin/Meshes/Yin"));
	primaryRefereeMesh->SetSkeletalMesh(obj01010.Object);

	courtMesh = CreateDefaultSubobject<UStaticMeshComponent>("Court");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj0(TEXT("/Game/Meshes/Court/StandardCourt"));
	courtMesh->SetStaticMesh(obj0.Object);
	courtMesh->SetNotifyRigidBodyCollision(true);
	courtMesh->SetHiddenInGame(true);
	SetRootComponent(courtMesh);

	courtOutOFBounds = CreateDefaultSubobject<UBoxComponent>("Court Player OB");
	courtOutOFBounds->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOutOFBounds->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtOutOFBounds->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel8);
	courtOutOFBounds->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	courtOutOFBounds->SetRelativeLocation(FVector(5.049217, 600.000000, 1000.000000));
	courtOutOFBounds->SetBoxExtent(FVector(1200.000000, 35.068577, 1278.008789));

	courtOutOFBounds2 = CreateDefaultSubobject<UBoxComponent>("Court Player OB2");
	courtOutOFBounds2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOutOFBounds2->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtOutOFBounds2->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel8);
	courtOutOFBounds2->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	courtOutOFBounds2->SetRelativeLocation(FVector(-44.416370, -600.000000, 1000.000000));
	courtOutOFBounds2->SetBoxExtent(FVector(1200.000000, 35.068577, 1278.008789));

	courtOutOFBounds3 = CreateDefaultSubobject<UBoxComponent>("Court Player OB3");
	courtOutOFBounds3->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOutOFBounds3->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtOutOFBounds3->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel8);
	courtOutOFBounds3->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	courtOutOFBounds3->SetRelativeLocation(FVector(-1200.000000, -4.296875, 1000.000000));
	courtOutOFBounds3->SetBoxExtent(FVector(1200.000000, 35.068577, 1278.008789));

	courtOutOFBounds4 = CreateDefaultSubobject<UBoxComponent>("Court Player OB4");
	courtOutOFBounds4->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOutOFBounds4->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtOutOFBounds4->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel8);
	courtOutOFBounds4->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	courtOutOFBounds4->SetRelativeLocation(FVector(1200.000000, -8.089661, 1000.000000));
	courtOutOFBounds4->SetBoxExtent(FVector(1200.000000, 35.068577, 1278.008789));

	courtOutOfBoundsLid = CreateDefaultSubobject<UBoxComponent>("Court lid");
	courtOutOfBoundsLid->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOutOfBoundsLid->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtOutOfBoundsLid->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel8);
	courtOutOfBoundsLid->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	courtOutOfBoundsLid->SetRelativeLocation(FVector(0, 0, 2000));
	courtOutOfBoundsLid->SetBoxExtent(FVector(1200.000000, 1200.000000, 50));

	playerASpawnLoc = CreateDefaultSubobject<USceneComponent>("PA Spawn");
	playerASpawnLoc->SetRelativeLocation(FVector(-499.999939, -199.999969, 20.000000));
	playerBSpawnLoc = CreateDefaultSubobject<USceneComponent>("PB Spawn");
	playerBSpawnLoc->SetRelativeLocation(FVector(500.000000, 200.000000, 20.000000));
	
	courtMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	courtOBFloor = CreateDefaultSubobject<UStaticMeshComponent>("Out of bounds floor");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj2(TEXT("/Game/Meshes/Court/StandardOBFloor"));
	courtOBFloor->SetStaticMesh(obj2.Object);

	courtOBFloor->SetupAttachment(courtMesh);
	courtOBFloor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	courtOBFloor->SetRelativeLocation(FVector::ZeroVector);
	courtOBFloor->SetHiddenInGame(true);

	centerNetMesh = CreateDefaultSubobject<UStaticMeshComponent>("Center Mesh");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj1(TEXT("/Game/Meshes/Island/All/all_Net"));
	centerNetMesh->SetStaticMesh(obj1.Object);
	centerNetMesh->SetRelativeRotation(FRotator(0,99.623,0));

	centerNetMesh->SetupAttachment(courtMesh);
	centerNetMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	centerNetMesh->SetRelativeLocation(FVector(0,0,97));
	centerNetMesh->SetNotifyRigidBodyCollision(true);

	playerACourtBoundary = CreateDefaultSubobject<UBoxComponent>("Player A Boundary");
	
	playerACourtBoundary->SetupAttachment(courtMesh);
	playerACourtBoundary->SetRelativeLocation(FVector::ZeroVector);
	playerACourtBoundary->SetRelativeLocation(FVector((-600.000000, 0.000000, 1000.000000)));
	playerACourtBoundary->SetBoxExtent(FVector(600.000000, 1200, 2000.000000));
	playerACourtBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);
	playerACourtBoundary->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	playerBCourtBoundary = CreateDefaultSubobject<UBoxComponent>("Player B Boundary");
	playerBCourtBoundary->SetupAttachment(courtMesh);
	playerBCourtBoundary->SetRelativeLocation(FVector(600.000000, 0.000000, 1000.000000));
	playerBCourtBoundary->SetBoxExtent(FVector(600.000000, 1200, 2000.000000));
	playerBCourtBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);
	playerBCourtBoundary->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	courtInBoundary = CreateDefaultSubobject<UBoxComponent>("Court in Boundary");
	courtInBoundary->SetupAttachment(courtMesh);
	courtInBoundary->SetCollisionResponseToAllChannels(ECR_Ignore);
	courtInBoundary->SetRelativeLocation(FVector(0, 0, 20.f));
	courtInBoundary->SetBoxExtent(FVector(593.984741, 300.000000, 34.504025));

	cleanSheet = CreateDefaultSubobject<UBoxComponent>("Cleaner Sheet");

	cleanSheet->SetupAttachment(courtMesh);
	cleanSheet->SetRelativeLocation(FVector(-3.999305, 0.074582, 825.000000));
	cleanSheet->SetBoxExtent(FVector(1.464963, 597.576965, 852.249146));
	cleanSheet->SetCollisionResponseToAllChannels(ECR_Ignore);
	cleanSheet->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	minimumSafeZ = CreateDefaultSubobject<USceneComponent>("Minimum Safe Z");
	minimumSafeZ->SetupAttachment(courtMesh);
	minimumSafeZ->SetRelativeLocation(FVector(0.000000, 0.000000, 139.000000));

	trainerAimLocation = CreateDefaultSubobject<USceneComponent>("Trainer Aim Location");
	trainerAimLocation->SetupAttachment(courtMesh);
	trainerAimLocation->SetRelativeLocation(FVector::ZeroVector);

	aimerMeshPlayerA = CreateDefaultSubobject<UStaticMeshComponent>("AimerA");
	aimerMeshPlayerA->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	aimerMeshPlayerA->SetupAttachment(courtMesh);
	aimerMeshPlayerA->SetRelativeLocation(FVector::ZeroVector);

	aimerMeshPlayerB = CreateDefaultSubobject<UStaticMeshComponent>("AimerB");
	aimerMeshPlayerB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	aimerMeshPlayerB->SetupAttachment(courtMesh);
	aimerMeshPlayerB->SetRelativeLocation(FVector::ZeroVector);

	playerAScoreText = CreateDefaultSubobject<UTextRenderComponent>("P A S");
	playerBScoreText = CreateDefaultSubobject<UTextRenderComponent>("P B S");
	ballBounceText = CreateDefaultSubobject<UTextRenderComponent>("text");
	playerBGameScoreText = CreateDefaultSubobject<UTextRenderComponent>("P B G S");
	playerAGameScoreText = CreateDefaultSubobject<UTextRenderComponent>("P A G S");
	gameOverText = CreateDefaultSubobject<UTextRenderComponent>("G O T");
	playerResponsibilityText = CreateDefaultSubobject<UTextRenderComponent>("P R T");

	courtHightPos = CreateDefaultSubobject<USceneComponent>("Court Min Height");
	courtHightPos->SetupAttachment(RootComponent);
	courtHightPos->SetRelativeLocation(FVector(0,0,-1));

	matchEnterComp = CreateDefaultSubobject<UBoxComponent>("Match start overlap");
	matchEnterComp->SetupAttachment(courtMesh);
	matchEnterComp->SetRelativeLocation(FVector(-17.204758, -456.034790, 65.000000));
	matchEnterComp->SetBoxExtent(FVector(132.608383, 46.702854, 149.112198));
	matchEnterComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	flagMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>("Flag mesh2");
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> obj3(TEXT("/Game/Meshes/Props/QBA_Standard"));
	flagMesh2->SetSkeletalMesh(obj3.Object);
	flagMesh2->SetRelativeLocation(FVector(-22.388977, 337.920044, -8.694061));
	flagMesh2->SetRelativeRotation(FRotator(0, 0, -180));
	flagMesh2->SetCollisionResponseToAllChannels(ECR_Ignore);

	musicComponent = CreateDefaultSubobject<UAudioComponent>("Music Component");
	musicComponent->SetupAttachment(RootComponent);
	musicComponent->SetRelativeLocation(FVector::ZeroVector);

	effectComponent = CreateDefaultSubobject<UAudioComponent>("Effect Component");

	indicateSpot1 = CreateDefaultSubobject<USceneComponent>("Indicator 1");
	indicateSpot2 = CreateDefaultSubobject<USceneComponent>("Indicator 2");

	indicateSpot1->SetRelativeLocation(FVector::ZeroVector);
	indicateSpot2->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> sys1(TEXT("/Game/Particles/Default/Ball_OB_Reset"));
	ballOBMarkSystem = sys1.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> sys2(TEXT("/Game/Particles/Default/Point_Score"));
	pointIndicatingSys = sys2.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> mus1(TEXT("/Game/Audio/Music/QuadBallMusic"));
	musicCue = mus1.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> mus2(TEXT("/Game/Audio/SFX/PointSound_Cue"));
	effectCue = mus2.Object;
	effectComponent->Sound = effectCue;

	static ConstructorHelpers::FObjectFinder<USoundCue> mus3(TEXT("/Game/Characters/Yin/Voice/Source/Yin_Ref_Voice"));
	refVoiceCue = mus3.Object;
	musicComponent->Sound = refVoiceCue;

	bReplicates = true;
}

#pragma endregion

#pragma region setup
// Called when the game starts or when spawned
void AQuadBallCourt::BeginPlay()
{
	Super::BeginPlay();

	musicComponent->Stop();
	effectComponent->Stop();
	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	if (primaryRefereeMesh)
	{
		mainRefAnimInstance = Cast<URefAnimInstance>(primaryRefereeMesh->GetAnimInstance());
	}

	CalculateCourtBasis();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAutoQuadBallCharacter::StaticClass(), allAisInWorld);
}

#pragma endregion

FString NetRoleToString1(ENetRole role)
{
	switch (role)
	{
	case ROLE_None:
		return FString("NONE: ");
		break;
	case ROLE_SimulatedProxy:
		return FString("Client: ");
		break;
	case ROLE_AutonomousProxy:
		return FString("Client: ");
		break;
	case ROLE_Authority:
		return FString("Server: ");
		break;
	case ROLE_MAX:
		return FString("Server: ");
		break;
	default:
		break;
	}
	return FString("NONE");
}

void Debug_PrintString(const int inval, const int key, const int time, const FString append, ENetRole Role)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString1(Role) + ":" + append + FString::FromInt(inval));
}

void Debug_PrintString(const float inval, const int key, const int time, const FString append, ENetRole Role)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString1(Role) + ":" + append + FString::SanitizeFloat(inval));
}

void Debug_PrintString(const FString &inval, const int key, const int time, const FString append, ENetRole Role)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString1(Role) + ":" + append + inval);
}

void AQuadBallCourt::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQuadBallCourt, playerAGameScore);
	DOREPLIFETIME(AQuadBallCourt, playerBGameScore);
	DOREPLIFETIME(AQuadBallCourt, playerAScore);
	DOREPLIFETIME(AQuadBallCourt, playerBScore);
	DOREPLIFETIME(AQuadBallCourt, nextGameServer);
	DOREPLIFETIME(AQuadBallCourt, needToSwitchSides);
	DOREPLIFETIME(AQuadBallCourt, needToEndGame);
	DOREPLIFETIME(AQuadBallCourt, gameMode);
	DOREPLIFETIME(AQuadBallCourt, inSession);
	DOREPLIFETIME(AQuadBallCourt, playersInQue);
	DOREPLIFETIME(AQuadBallCourt, playerA);
	DOREPLIFETIME(AQuadBallCourt, playerB);
	DOREPLIFETIME(AQuadBallCourt, ballStartSide);
	DOREPLIFETIME(AQuadBallCourt, playerBallResponsibility);
	DOREPLIFETIME(AQuadBallCourt, playerAName);
	DOREPLIFETIME(AQuadBallCourt, playerBName);
}

void AQuadBallCourt::OnRep_playerAScore()
{
	//UpdatePlayerScore();
}

void AQuadBallCourt::OnRep_playerBScore()
{
	//UpdatePlayerScore();
}

void AQuadBallCourt::OnRep_playerAGameScore()
{
	//UpdatePlayerScore();
}

void AQuadBallCourt::OnRep_playerBGameScore()
{
	//UpdatePlayerScore();
}

// This actor never ticks
void AQuadBallCourt::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (inSession) return;
	if (refGreetCooldown > 0)
	{
		refGreetCooldown -= DeltaTime;
	}
}

void AQuadBallCourt::LookAt(AQuadBallCharacter* targChar, bool look)
{
	if (look && targChar)
	{
		if (inSession) return;
		mainRefAnimInstance->SetLooking(true, Cast<USceneComponent>(targChar->GetMesh()));
		if (refGreetCooldown <= 0)
		{
			musicComponent->Play();
			refGreetCooldown = refGreetMax;
		}

	}
	else
	{
		mainRefAnimInstance->SetLooking(false, nullptr);
	}
}

#pragma region start and end game

AQuadBallCharacter* AQuadBallCourt::FetchOtherPlayer(AQuadBallCharacter* player)
{
	int attemptsMax = allAisInWorld.Num();
	AQuadBallCharacter* otherChar = nullptr;
	for (auto el : allAisInWorld)
	{
		otherChar = Cast<AQuadBallCharacter>(el);
		if (otherChar != player && !otherChar->GetIsPlaying()) break;
		else otherChar = nullptr;
	}

	return otherChar;
}

// Enter the queue for the game, 2 players required to play.  If the court is an AI match court, the second player will be automatically asigned
void AQuadBallCourt::MULTICAST_SetPlayerA_Implementation(AQuadBallCharacter* player)
{
	playerA = player;
	playerA->SetActiveBall(ball);
	playerA->SetActiveBall(this);
}

void AQuadBallCourt::MULTICAST_SetPlayerB_Implementation(AQuadBallCharacter* player)
{
	playerB = player;
	playerB->SetActiveBall(ball);
	playerB->SetActiveBall(this);
}

bool AQuadBallCourt::Server_QueForGame_Validate(AQuadBallCharacter* player, bool challenge) { return true; }

void AQuadBallCourt::Server_QueForGame_Implementation(AQuadBallCharacter* player, bool challenge)
{
	if (playersInQue == 0)
	{
		playersInQue++;

		MULTICAST_SetPlayerA(player);

		if (AICourtType && !challenge)
		{
			AQuadBallCharacter* otherChar = FetchOtherPlayer(player);

			if (!otherChar)
			{
				player->Client_NoOtherPlayers();
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString("No other character available"));
				playersInQue--;
				return;
			}

			Server_QueForGame(otherChar);
		}
	}
	else if (playersInQue == 1)
	{
		if (player == playerA) return;
		MULTICAST_SetPlayerB(player);
		playersInQue++;
		Server_StartMatch();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Emerald, FString::FromInt(playersInQue));
}

bool AQuadBallCourt::Server_SetupPlayerForQuadBall_Validate(AQuadBallCharacter* player, const int playerID) { return true; }

void AQuadBallCourt::Server_SetupPlayerForQuadBall_Implementation(AQuadBallCharacter* player, const int playerID)
{
	player->Client_SetMusic(musicCue, true);
	player->SetChallengeRequestActive(false);
	player->Client_InitPlayerForQuadBall(ball, this);
	player->SetIsPlaying(true);
	player->SetGameMode(1);
	player->SetPlayerID(playerID);
}

bool AQuadBallCourt::Server_StartMatch_Validate() { return true; }

void AQuadBallCourt::Server_StartMatch_Implementation()
{
	if (!playerA || !playerB)
	{
		GEngine->AddOnScreenDebugMessage(-1, 500, FColor::Emerald, FString("MISSING PLAYERS"));
		return;
	}

	playerAName = playerA->GetQBName();
	playerBName = playerB->GetQBName();

	playerA->Server_SetInitQuadBallOrientation(playerASpawnLoc->GetComponentLocation(), courtMesh->GetForwardVector().ToOrientationRotator());
	playerB->Server_SetInitQuadBallOrientation(playerBSpawnLoc->GetComponentLocation(), (GetCourtFloor()->GetForwardVector() * -1).ToOrientationRotator());

	inSession = true;
	firstInteract = false;
	needToSwitchSides = false;
	needToEndGame = false;

	MULTICAST_StartMatch(true);

	Server_SetupPlayerForQuadBall(playerA, 0);
	Server_SetupPlayerForQuadBall(playerB, 1);

	playerA->SetOpponentIsAI(playerB->GetSelfIsAI());
	playerB->SetOpponentIsAI(playerA->GetSelfIsAI());

	playerAScore = 0;
	playerBScore = 0;
	playerAGameScore = 0;
	playerBGameScore = 0;

	int randStart = FMath::RandRange(0, 1);
	ballStartSide = randStart;

	if (ballStartSide == 0) playerA->MULTICAST_Reset();
	else playerB->MULTICAST_Reset();

	ball->Server_RequestBallReset();
	ball->InitPlayerBallResponsibility(randStart);
	ball->SetNewServe(true);
	ball->MULTICAST_SetPositionIndicatorVisibleStatus(true);


	ballStartSide = randStart;
	nextGameServer = randStart;

	MULTICAST_UpdatePlayerScore();

}

// Initiate quad ball game with 2 valid players
void AQuadBallCourt::MULTICAST_StartMatch_Implementation(const bool vsAI)
{
	mainRefAnimInstance->SetLooking(true, Cast<USceneComponent>(ball->GetBallMesh()));



	courtOutOFBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	courtOutOFBounds2->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	courtOutOFBounds3->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	courtOutOFBounds4->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	gameOverText->SetText(FText::FromString(FString("Game\nin progress")));
	playerA->SetGameCollision(true);
	playerB->SetGameCollision(true);
}

bool AQuadBallCourt::Server_EndMatch_Validate() { return true; }

void AQuadBallCourt::Server_EndMatch_Implementation()
{
	inSession = false;
	playerA->Client_InitPlayerForIdle();
	playerB->Client_InitPlayerForIdle();
	playerA->Client_UI_HideGameUI();
	playerB->Client_UI_HideGameUI();

	playerA->SetGameMode(0);
	playerA->SetIsPlaying(false);
	playerB->SetGameMode(0);
	playerB->SetIsPlaying(false);

	playerA->Client_SetMusic(nullptr, false);
	playerB->Client_SetMusic(nullptr, false);


	MULTICAST_HidePoint();
	ball->GetBallMesh()->SetSimulatePhysics(true);
	ball->SetBallPlayStatus(0);
	ball->MULTICAST_SetPositionIndicatorVisibleStatus(false);
	ball->GetBallMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	ball->GetBallMesh()->SetPhysicsLinearVelocity(FVector::UpVector * 500);
	courtOutOFBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds2->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds3->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds4->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	MULTICAST_EndMatch();
	MULTICAST_CancelIndicateParticles();
	playersInQue = 0;
}

// End quad ball game
void AQuadBallCourt::MULTICAST_EndMatch_Implementation()
{
	courtOutOFBounds->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds2->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds3->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	courtOutOFBounds4->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	mainRefAnimInstance->SetLooking(false, nullptr);
}

// Check if an end of game is required
bool AQuadBallCourt::CheckEndGame()
{
	if (needToEndGame)
	{
		Server_EndMatch();
		needToEndGame = false;
		return true;
	}

	return false;
}

// Check if a switch of sides is required
void AQuadBallCourt::CheckSwitchSide()
{
	if (needToSwitchSides && !needToEndGame)
	{
		Server_SwitchSides();
		needToSwitchSides = false;
	}
}

// Execute switch of sides
bool AQuadBallCourt::Server_SwitchSides_Validate() { return true; }

void AQuadBallCourt::Server_SwitchSides_Implementation()
{
	int t = playerBGameScore;
	FString t2 = playerAName;

	playerBGameScore = playerAGameScore;
	playerAGameScore = t;
	AQuadBallCharacter* temp1 = playerA;

	MULTICAST_SetPlayerA(playerB);
	MULTICAST_SetPlayerB(temp1);

	playerA->SetPlayerID(0);
	playerB->SetPlayerID(1);
	//Server_SetupPlayerForQuadBall(playerA, 0);
	//Server_SetupPlayerForQuadBall(playerB, 1);

	playerAName = playerBName;
	playerBName = t2;

	if (playerA) playerA->Server_SetInitQuadBallOrientation(playerASpawnLoc->GetComponentLocation(), courtMesh->GetForwardVector().ToOrientationRotator());
	if (playerB) playerB->Server_SetInitQuadBallOrientation(playerBSpawnLoc->GetComponentLocation(), (courtMesh->GetForwardVector() * -1).ToOrientationRotator());

	if (playerA->GetSelfIsAI() || playerB->GetSelfIsAI()) ball->SetReadyToServe(true);

	MULTICAST_UpdatePlayerScore();
	ball->InitPlayerBallResponsibility(nextGameServer);
	ballStartSide = nextGameServer;
	ball->SetBallPlayStatus(0);
}


#pragma endregion

#pragma region initialization

void AQuadBallCourt::CalculateCourtBasis()
{
	courtForward = courtMesh->GetForwardVector();
	courtForward.Normalize();
	courtRight = courtMesh->GetRightVector();
	courtRight.Normalize();
	courtUp = courtMesh->GetUpVector();
	courtUp.Normalize();
	courtBasisMatrix = FBasisVectorMatrix(courtForward, courtRight, courtUp, courtInBoundary->GetComponentLocation());

	// Determine court extents from court boundary box
	FVector boxExt = courtInBoundary->GetScaledBoxExtent();
	FVector boxLoc = courtInBoundary->GetComponentLocation();

	// Establish extrememe points of boundary box
	topRight = boxLoc + ((courtMesh->GetForwardVector() * boxExt.X) + (courtMesh->GetRightVector() * boxExt.Y));
	bottomLeft = boxLoc - ((courtMesh->GetForwardVector() * boxExt.X) + courtMesh->GetRightVector() * boxExt.Y);

	topRight = courtBasisMatrix.InverseTransformPosition(topRight);
	bottomLeft = courtBasisMatrix.InverseTransformPosition(bottomLeft);

	// top right and bottom left may switch depending on the orientation

	if (topRight.X < bottomLeft.X)
	{
		FVector t = topRight;
		topRight = bottomLeft;
		bottomLeft = t;
	}

	/*
	//GEngine->AddOnScreenDebugMessage(9991, 1, FColor::Emerald, FString(thisLoc.ToString() + "\n" + topRight.ToString() + "\n" + bottomLeft.ToString())); FScreenMessageString* m0 = GEngine->ScreenMessages.Find(9991); m0->TextScale.X = m0->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(9992, 1, FColor::Emerald, FString("X: ") + FString::FromInt(thisLoc.X > topRight.X) + "," + FString::FromInt(thisLoc.X < bottomLeft.X) + FString("Y: ") + FString::FromInt(thisLoc.Y > topRight.Y) + "," + FString::FromInt(thisLoc.Y < bottomLeft.Y)); FScreenMessageString* m1 = GEngine->ScreenMessages.Find(9992); m1->TextScale.X = m1->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(9993, 1, FColor::Emerald, FString::FromInt(isInside)); FScreenMessageString* m2 = GEngine->ScreenMessages.Find(9993); m2->TextScale.X = m2->TextScale.Y = 1.0f;

	//a.DebugPrint();


	GEngine->AddOnScreenDebugMessage(9991, 1, FColor::Emerald, FString("PRE\n[") + FString::SanitizeFloat(courtForward.X) + "," + FString::SanitizeFloat(courtRight.X) + "," + FString::SanitizeFloat(courtUp.X) + "]"); FScreenMessageString* m0 = GEngine->ScreenMessages.Find(9991); m0->TextScale.X = m0->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(9992, 1, FColor::Emerald, FString("[") + FString::SanitizeFloat(courtForward.Y) + "," + FString::SanitizeFloat(courtRight.Y) + "," + FString::SanitizeFloat(courtUp.Y) + "]"); FScreenMessageString* m1 = GEngine->ScreenMessages.Find(9992); m1->TextScale.X = m1->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(9993, 1, FColor::Emerald, FString("[") + FString::SanitizeFloat(courtForward.Z) + "," + FString::SanitizeFloat(courtRight.Z) + "," + FString::SanitizeFloat(courtUp.Z) + "]\n"); FScreenMessageString* m2 = GEngine->ScreenMessages.Find(9993); m2->TextScale.X = m2->TextScale.Y = 1.0f;

	FVector::CreateOrthonormalBasis(courtForward, courtRight, courtUp);

	GEngine->AddOnScreenDebugMessage(9994, 1, FColor::Emerald, FString("Post\n[") + FString::SanitizeFloat(courtForward.X) + "," + FString::SanitizeFloat(courtRight.X) + "," + FString::SanitizeFloat(courtUp.X) + "]"); FScreenMessageString* m3 = GEngine->ScreenMessages.Find(9994); m3->TextScale.X = m3->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(9995, 1, FColor::Emerald, FString("[") + FString::SanitizeFloat(courtForward.Y) + "," + FString::SanitizeFloat(courtRight.Y) + "," + FString::SanitizeFloat(courtUp.Y) + "]"); FScreenMessageString* m4 = GEngine->ScreenMessages.Find(9995); m4->TextScale.X = m4->TextScale.Y = 1.0f;
	GEngine->AddOnScreenDebugMessage(9996, 1, FColor::Emerald, FString("[") + FString::SanitizeFloat(courtForward.Z) + "," + FString::SanitizeFloat(courtRight.Z) + "," + FString::SanitizeFloat(courtUp.Z) + "]"); FScreenMessageString* m5 = GEngine->ScreenMessages.Find(9996); m5->TextScale.X = m5->TextScale.Y = 1.0f;
	*/
}

#pragma endregion

#pragma region utilities
// Determine which side of the court the ball currently lies, only updated when the ball is completely on one side
int AQuadBallCourt::GetBallSide(AQuadBallBall* ball)
{
	// TEST
	//GEngine->AddOnScreenDebugMessage(101, 1, FColor::Blue, FString("BALL OVERLAPPING A: ") + FString::FromInt(playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))); FScreenMessageString* m101 = GEngine->ScreenMessages.Find(101); m101->TextScale.X = m101->TextScale.Y = 1.0f;
	//GEngine->AddOnScreenDebugMessage(102, 1, FColor::Blue, FString("BALL OVERLAPPING B: ") + FString::FromInt(playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))); FScreenMessageString* m102 = GEngine->ScreenMessages.Find(102); m102->TextScale.X = m102->TextScale.Y = 1.0f;

	if (playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()) && !playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))
	{
		return 0;
	}
	else if (playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()) && !playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))
	{
		return 1;
	}
	else if (!playerACourtBoundary->IsOverlappingComponent(ball->GetBallMesh()) && !playerBCourtBoundary->IsOverlappingComponent(ball->GetBallMesh()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString("Ball side not found"));
		ball->DebugResetBall();
		return -1;
	}

	return ball->GetCurrentBallSide();
}

void AQuadBallCourt::MULTICAST_AwardPointEffect_Implementation(const int playerTarget, const FString &reason, const bool award)
{
	FString dispName = "";
	if (award)
	{
		if ((playerTarget) % 2 == 0)
		{
			mainRefAnimInstance->PointA();

		}
		else if ((playerTarget) % 2 == 1)
		{
			mainRefAnimInstance->PointB();
		}

		(playerTarget) % 2 == 0 ? dispName = FString(playerAName) : dispName = FString(playerBName);
		gameOverText->SetText(FText::FromString(dispName + " Point!\n " + reason));

		if ((playerAScore >= pointsToWinRound && playerAScore - playerBScore >= gapToWinRound) || playerAScore >= 10)
		{
			gameOverText->SetText(FText::FromString(playerAName + FString(" WINS GAME!")));
		}
		else if ((playerBGameScore >= pointsToWinGame && playerBGameScore - playerAGameScore >= gapToWinGame) || playerBGameScore >= 2)
		{
			gameOverText->SetText(FText::FromString(playerBName + FString(" WINS GAME!")));
		}

		PlayIndicateParticles();
	}
	else
	{
		(playerTarget + 1) % 2 == 0 ? dispName = FString(playerAName) : dispName = FString(playerBName);
		gameOverText->SetText(FText::FromString(dispName + " Retry Serve!\n " + reason));
	}

	effectComponent->Play();
}

// Award a point to given player for indicated reason
bool AQuadBallCourt::Server_AwardPoint_Validate(const int playerTarget, const FString &reason) { return true; }

void AQuadBallCourt::Server_AwardPoint_Implementation(const int playerTarget, const FString &reason)
{
	MULTICAST_AwardPointEffect(playerTarget, reason, !ball->GetNewServe());

	if (!ball->GetNewServe())
	{
		if ((playerTarget) % 2 == 0)
		{
			playerAScore += 1;
			ballStartSide = 0;
		}
		else if ((playerTarget) % 2 == 1)
		{
			playerBScore += 1;
			ballStartSide = 1;
		}

		// Reset the ball to a new serve
		ball->SetNewServe(true);

		// Determine winning conditions, not put into effect until player resets the point
		if ((playerAScore >= pointsToWinRound && playerAScore - playerBScore >= gapToWinRound) || playerAScore >= 10)
		{
			playerAGameScore += 1;
			playerAScore = 0;
			playerBScore = 0;
			nextGameServer = 0;
			needToSwitchSides = true;
			playerA->Server_PlayWin(1);
			playerB->Server_PlayWin(0);
		}
		else if ((playerBScore >= pointsToWinRound && playerBScore - playerAScore >= gapToWinRound) || playerBScore >= 10)
		{
			playerBGameScore += 1;
			playerAScore = 0;
			playerBScore = 0;
			nextGameServer = 1;
			needToSwitchSides = true;
			playerB->Server_PlayWin(1);
			playerA->Server_PlayWin(0);
		}

		if ((playerAGameScore >= pointsToWinGame && playerAGameScore - playerBGameScore >= gapToWinGame) || playerAGameScore >= 2)
		{
			playerA->Server_PlayWin(2);
			needToEndGame = true;
		}
		else if ((playerBGameScore >= pointsToWinGame && playerBGameScore - playerAGameScore >= gapToWinGame) || playerBGameScore >= 2)
		{
			playerB->Server_PlayWin(2);
			needToEndGame = true;
		}
	}
	else
	{
		// For "first serve" no point awarded
		if (playerTarget == 0)
		{
			ballStartSide = 1;
		}
		else if (playerTarget == 1)
		{
			ballStartSide = 0;
		}
		ball->SetNewServe(false);
	}

	ball->SetBallPlayStatus(0);
	ball->SetNeedsToBeReset(true);
	MULTICAST_UpdatePlayerScore();
}

// Depricated
void AQuadBallCourt::MULTICAST_HidePoint_Implementation()
{
	mainRefAnimInstance->ClearPoint();
}

// Resets all current player used kick flags
bool AQuadBallCourt::Server_ResetAllPlayerKicks_Validate() { return true; }

void AQuadBallCourt::Server_ResetAllPlayerKicks_Implementation()
{
	playerA->Server_ResetAllKicks();
	playerB->Server_ResetAllKicks();
}

// Returns minimum z value for a valid pass over the net
FVector AQuadBallCourt::GetMinimumSafeZLocation()
{
	return minimumSafeZ->GetComponentLocation();
}

// Depricated
FVector AQuadBallCourt::GetTrainerAimLocation()
{
	return trainerAimLocation->GetComponentLocation();
}

// Depricated
void AQuadBallCourt::ResetTrainerReturn()
{

}

// Identify the responsible player on the scoreboard
void AQuadBallCourt::MULTICAST_UpdatePlayerResponsibilityDisplay_Implementation(const int resp)
{
	if (resp == 0)
	{
		playerResponsibilityText->SetText(FText::FromString("<"));
		/*
		if (firstInteract)
		{
			if (playerA)
			{
				playerA->UI_ChangePlayerResponsible(Cast<ACharacter>(playerA));
				if (playerB) playerB->UI_ChangePlayerResponsible(Cast<ACharacter>(playerA));
			}
			else
			{
				if (playerB) playerB->UI_ChangePlayerResponsible(Cast<ACharacter>(trainer));
			}
		}
		*/
	}
	else
	{
		playerResponsibilityText->SetText(FText::FromString(">"));
		/*
		if (firstInteract)
		{
			if (playerB)
			{
				playerB->UI_ChangePlayerResponsible(Cast<ACharacter>(playerB));
				if (playerA) playerA->UI_ChangePlayerResponsible(Cast<ACharacter>(playerB));
			}
			else
			{
				if (playerA) playerA->UI_ChangePlayerResponsible(Cast<ACharacter>(trainer));
			}
		}
		*/
	}
}

// Update ball bounce display on scoreboard
void AQuadBallCourt::MULTICAST_UpdateBallBounceDisplay_Implementation(const int now, int max)
{
	if (ball->GetIsServe()) max = 1;
	ballBounceText->SetText(FText::FromString(FString::FromInt(now) + FString("/") + FString::FromInt(max)));
}

// Update player score display on scoreboard
void AQuadBallCourt::MULTICAST_UpdatePlayerScore_Implementation()
{
	playerAScoreText->SetText(FText::FromString(playerAName + ":\nGame: " + FString::FromInt(playerAGameScore) + "\nRound: " + FString::FromInt(playerAScore)));
	playerBScoreText->SetText(FText::FromString(playerBName + ":\nGame: " + FString::FromInt(playerBGameScore) + "\nRound: " + FString::FromInt(playerBScore)));
}

// Return a world space location of a point on the court given by a percentage horizontal and percentage vertical. myC determines the direction of the court (postive for court side B)
FVector AQuadBallCourt::GetPositionOnCourt(const int playerID, const float x, const float y, const int myC)
{
	float errorAmount = 1;
	FVector r;
	if (playerID == 0)
	{
		aimerMeshPlayerA->SetRelativeLocation(FVector(myC * courtHight * y * errorAmount,(.5 * courtWidth) * x * errorAmount, 50));
		r = aimerMeshPlayerA->GetComponentLocation();
	}
	else
	{
		aimerMeshPlayerB->SetRelativeLocation(FVector(myC * -courtHight * y * errorAmount,(.5 * courtWidth) * x * errorAmount, 50));
		r = aimerMeshPlayerB->GetComponentLocation();
	}
	
	return r;
}


void AQuadBallCourt::Client_StartMusic_Implementation()
{

}

void AQuadBallCourt::Client_EndMusic_Implementation()
{

}

// Mark the out of bounds location using the flag
void AQuadBallCourt::MarkOBLocation(FVector &loc)
{
	loc.Z = courtHightPos->GetComponentLocation().Z;
	flagMesh2->SetWorldLocation(loc);
	ballOBMarkSystemComponent = UGameplayStatics::SpawnEmitterAttached(ballOBMarkSystem, flagMesh2);
}

// Depricated
void AQuadBallCourt::ShowVSAnimForAllPlayers()
{
	if (playerA) playerA->UI_ShowVSAnim();
	if (playerB) playerB->UI_ShowVSAnim();


	firstInteract = true;
}

// Depricated
void AQuadBallCourt::SetPlayersForAllPlayers()
{
	//if (playerA) playerA->UI_SetPlayers();
	//if (playerB) playerB->UI_SetPlayers();
}

// Depricated
bool AQuadBallCourt::GetBallSafeDistanceFromNet()
{
	FVector trueDist = (minimumSafeZ->GetComponentLocation() * courtMesh->GetForwardVector()) - (ball->GetBallMesh()->GetComponentLocation()* courtMesh->GetForwardVector());

	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Emerald, FString::SanitizeFloat(trueDist.Size()));

	if (trueDist.Size() > minSafeNetDist) return false;
	else return true;
}

#pragma endregion

#pragma region settings
// Set music volume
void AQuadBallCourt::ChangeMusicVolume(const float percent)
{
	musicComponent->SetFloatParameter(MUS_VOL, percent);
}

// Set master volume
void AQuadBallCourt::ChangeMasterVolume(const float percent)
{
	musicComponent->SetFloatParameter(MAST_VOL, percent);
}

// Set SFX volume
void AQuadBallCourt::ChangeSFXVolume(const float percent)
{
	effectComponent->SetFloatParameter(SFX_VOL, percent);
}

#pragma endregion

#pragma region particles

// Play point particles, looping, not self distructive
void AQuadBallCourt::PlayIndicateParticles()
{
	pointIndicatingComp1 = UGameplayStatics::SpawnEmitterAttached(pointIndicatingSys, indicateSpot1);
	pointIndicatingComp2 = UGameplayStatics::SpawnEmitterAttached(pointIndicatingSys, indicateSpot2);
}

// Cancel point particles if they exist, no effect if they are not active.
void AQuadBallCourt::MULTICAST_CancelIndicateParticles_Implementation()
{
	if (pointIndicatingComp1) pointIndicatingComp1->DestroyComponent();
	if (pointIndicatingComp2) pointIndicatingComp2->DestroyComponent();
}

#pragma endregion
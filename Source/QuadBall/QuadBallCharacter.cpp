// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
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
#include "UnrealNetwork.h"
#include "Components/SplineMeshComponent.h"

#pragma region construct
// Sets default values
AQuadBallCharacter::AQuadBallCharacter()
{
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel9, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel10, ECR_Overlap);

	playerRotatePoint = CreateDefaultSubobject<USceneComponent>("Player rotate point");
	playerRotatePoint->SetupAttachment(RootComponent);
	playerRotatePoint->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> obj0(TEXT("/Game/Characters/Yin/Yin_QuadBall_AnimBP"));
	animInstanceSourceClass = obj0.Class;

	static ConstructorHelpers::FClassFinder<UQuadBallCharacterAnimInstance> obj99(TEXT("/Game/Characters/Yin/Yin_Real_Menu_AnimBP"));
	menuAnimInstanceSourceClass = obj99.Class;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> obj1(TEXT("/Game/ParagonYin/Characters/Heroes/Yin/Meshes/Yin"));
	playerSourceMesh = obj1.Object;

	static ConstructorHelpers::FClassFinder<UUserWidget> obj2(TEXT("/Game/UI/Widgets/QuadBall_InWorldUI_BP"));
	defaultWorldUIClass = obj2.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> obj3(TEXT("/Game/UI/Widgets/QuadBallUI_BP"));
	UIWidgetBase = obj3.Class;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> obj4(TEXT("/Game/Meshes/Props/AimerCone"));

	//GetMesh()->SetRelativeLocation(FVector::ZeroVector);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetupAttachment(RootComponent);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -100));
	GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
	GetMesh()->SetWorldScale3D(FVector(.88, .88, .88));
	GetMesh()->SetSkeletalMesh(playerSourceMesh);
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);

	playerCameraBoom = CreateDefaultSubobject<USpringArmComponent>("Player Camera Boom");
	playerCameraBoom->SetupAttachment(RootComponent);
	playerCameraBoom->TargetArmLength = 200;
	playerCameraBoom->SetRelativeRotation(FRotator(cameraPitchNormal, 0, 0));
	playerCameraBoom->bEnableCameraLag = true;
	playerCameraBoom->bDoCollisionTest = false;

	playerCamera = CreateDefaultSubobject<UCameraComponent>("Player camera");
	playerCamera->SetupAttachment(playerCameraBoom, USpringArmComponent::SocketName);
	playerCamera->SetRelativeLocation(FVector::ZeroVector);
	playerCamera->SetRelativeRotation(FRotator::ZeroRotator);
	playerCamera->bUsePawnControlRotation = false;

	ballInteractBoundBodyBlock = CreateDefaultSubobject<UBoxComponent>("Ball interact boundary for body");
	ballInteractBoundBodyBlock->SetupAttachment(RootComponent);
	ballInteractBoundBodyBlock->SetRelativeLocation(FVector::ZeroVector);
	ballInteractBoundBodyBlock->SetRelativeLocation(FVector(5, 0, -20));
	ballInteractBoundBodyBlock->SetBoxExtent(FVector(15.89, 28.25, 73.41));
	ballInteractBoundBodyBlock->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ballInteractBoundBodyBlock->SetCollisionResponseToAllChannels(ECR_Ignore);
	ballInteractBoundBodyBlock->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	challengeOverlap = CreateDefaultSubobject<USphereComponent>("Challenge overlap");
	challengeOverlap->SetupAttachment(RootComponent);
	challengeOverlap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	challengeOverlap->SetRelativeLocation(FVector::ZeroVector);
	challengeOverlap->SetSphereRadius(100.f);
	challengeOverlap->SetCollisionObjectType(ECC_GameTraceChannel10);
	challengeOverlap->SetCollisionResponseToAllChannels(ECR_Ignore);

	playerAimerComp = CreateDefaultSubobject<UStaticMeshComponent>("Player's aimer");
	playerAimerComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	playerAimerComp->SetupAttachment(RootComponent);
	playerAimerComp->SetRelativeLocation(FVector::ZeroVector);
	playerAimerComp->SetStaticMesh(obj4.Object);

	voiceComp = CreateDefaultSubobject<UAudioComponent>("Voice Audio Comp");
	voiceComp->SetupAttachment(RootComponent);
	voiceComp->SetRelativeLocation(FVector::ZeroVector);

	inWorldUI = CreateDefaultSubobject<UWidgetComponent>("World UI");
	inWorldUI->SetupAttachment(GetCapsuleComponent());
	inWorldUI->SetRelativeLocation(FVector::ZeroVector);
	inWorldUI->SetWidgetClass(defaultWorldUIClass);
	inWorldUI->SetRelativeLocation(FVector(0, 0, -80));
	inWorldUI->SetRelativeRotation(FRotator(90, 180, 0));
	inWorldUI->SetRelativeScale3D(FVector(.5, .5, .5));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve1(TEXT("/Game/Curves/Def/Vel1"));
	characterSpecialVelCurve = curve1.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve2(TEXT("/Game/Curves/Def/Vel2"));
	characterSpecialVelCurve2= curve2.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve3(TEXT("/Game/Curves/Def/X1"));
	characterSpecialXCurve = curve3.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve4(TEXT("/Game/Curves/Def/X2"));
	characterSpecialXCurve2 = curve4.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve5(TEXT("/Game/Curves/Def/Z1"));
	characterSpecialZCurve = curve5.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve6(TEXT("/Game/Curves/Def/Z2"));
	characterSpecialZCurve2 = curve6.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> curve7(TEXT("/Game/Curves/Def/BoostAccel"));
	boostKickAccelCurve = curve7.Object;
	static ConstructorHelpers::FObjectFinder<UCurveVector> curve8(TEXT("/Game/Curves/Def/Sppecial_Curve1"));
	characterSpecialCurve1 = curve8.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> part1(TEXT("/Game/Particles/Default/Def_Charging_Particles"));
	chargingEffect = part1.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> part2(TEXT("/Game/Particles/Default/Def_SuperActive_Particles"));
	specialActiveEffect = part2.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> part3(TEXT("/Game/Particles/Default/Def_BallImpact_Particles"));
	specialBallHitEffect = part3.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> part4(TEXT("/Game/Particles/Default/Def_Burst_Particles"));
	specialPlayerBurst = part4.Object;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> part5(TEXT("/Game/Particles/Default/Def_GameWin_Particles"));
	gameWinEffect = part5.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial> mat1(TEXT("/Game/UI/Character_Portraits/Yin_Spash"));
	characterPortrait = mat1.Object;

	effectComp = CreateDefaultSubobject<UAudioComponent>("Effect component");
	effectComp->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USoundCue> cue1(TEXT("/Game/Audio/SFX/BallHit"));
	effectCue = cue1.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> cue2(TEXT("/Game/Characters/Yin/Voice/Voice1/Yin_Paragon_Voice"));
	voiceCue = cue2.Object;

	effectComp->SetSound(effectCue);
	voiceComp->SetSound(voiceCue);

	musicComp = CreateDefaultSubobject<UAudioComponent>("Music comp");

	localPlayerID = -99;
	chargeMult = 1;
	cameraPitchNormal = -15;
	maxInteractDist = 55;
	rotateSpeed = 75;
	minCharge = .3;
	bodyMult = .3;
	headMult = 1;
	boostVel = 10;

	bReplicates = true;
	bReplicateMovement = true;
}

#pragma endregion

#pragma region setup
// Called when the game starts or when spawned
void AQuadBallCharacter::BeginPlay()
{
	Super::BeginPlay();
	localPlayerID = 1;
	
	// Stop sound effect components
	effectComp->SetRelativeLocation(FVector::ZeroVector);
	effectComp->Stop();
	voiceComp->SetRelativeLocation(FVector::ZeroVector);
	voiceComp->Stop();

	// Hide aim target reference
	GetMesh()->SetHiddenInGame(false);
	playerAimerComp->SetHiddenInGame(true);
	canControlMove = true;
	playerAimerComp->bOnlyOwnerSee = true;
	playerCameraBoom->SetRelativeRotation(FRotator(cameraPitchNormal, 0, 0));
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());
	if (!gameInstance) return;
	if (gameInstance->gameMode == -1) return;

	if (HasAuthority())
	{
		Server_SetName(GetName());
	}

	if (IsLocallyControlled())
	{
		UpdateAllVolume();
		WorldUIWidget = Cast<UQuadBallUI>(inWorldUI->GetUserWidgetObject());
		UIWidget = Cast<UQuadBallUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
		UIWidget->SetOwner(this);
		UIWidget->AddToViewport();
		UIWidget->SetVolumeSliders(gameInstance->defaultSFXVol, gameInstance->defaultMusicVol, gameInstance->defaultMasterVol);
	}

	// Init remaining values
	chargeAmount = minCharge;

	aimX = .5;
	aimY = .5;

	// Bind delegates for overlap
	ballInteractBoundBodyBlock->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapBeginBodyBlock);
	ballInteractBoundBodyBlock->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnOverlapEndBallBodyBlock);

	//challengeOverlap->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnChallengeOverlapBegin);
	//challengeOverlap->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnChallengeOverlapEnd);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AQuadBallCharacter::OnCapsuleOverlapInteractableBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AQuadBallCharacter::OnCapsuleOverlapInteractableEnd);
}

// Called to bind functionality to input
void AQuadBallCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Reset", IE_Pressed, this, &AQuadBallCharacter::ResetFromPlayer);
	PlayerInputComponent->BindAction("Invert", IE_Pressed, this, &AQuadBallCharacter::AlternateTargetCourtSide);
	DECLARE_DELEGATE_OneParam(ChargeR, const int);
	PlayerInputComponent->BindAction<ChargeR>("KickR", IE_Pressed, this, &AQuadBallCharacter::Charge, 2);
	DECLARE_DELEGATE_OneParam(ChargeL, const int);
	PlayerInputComponent->BindAction<ChargeL>("KickL", IE_Pressed, this, &AQuadBallCharacter::Charge, 1);
	DECLARE_DELEGATE_OneParam(ChargeH, const int);
	PlayerInputComponent->BindAction<ChargeH>("Head", IE_Pressed, this, &AQuadBallCharacter::Charge, 5);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AQuadBallCharacter::DoJump);
	//PlayerInputComponent->BindAction("BodyBlock", IE_Pressed, this, &AQuadBallCharacter::BodyBlock);
	PlayerInputComponent->BindAction("Switch", IE_Pressed, this, &AQuadBallCharacter::HandstandModeTransition);
	PlayerInputComponent->BindAction("DebugUp", IE_Pressed, this, &AQuadBallCharacter::DebugUp);
	PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AQuadBallCharacter::Pause);
	PlayerInputComponent->BindAction("BoostKickR", IE_Pressed, this, &AQuadBallCharacter::BoostKickRight);
	PlayerInputComponent->BindAction("BoostKickL", IE_Pressed, this, &AQuadBallCharacter::BoostKickLeft);
	DECLARE_DELEGATE_OneParam(FSpecialIn, const int);
	PlayerInputComponent->BindAction<FSpecialIn>("Special1", IE_Pressed, this, &AQuadBallCharacter::SetSpecialMove, 1);
	DECLARE_DELEGATE_OneParam(FSpecialIn2, const int);
	PlayerInputComponent->BindAction<FSpecialIn2>("Special2", IE_Pressed, this, &AQuadBallCharacter::SetSpecialMove, 2);
	PlayerInputComponent->BindAction("Challenge", IE_Pressed, this, &AQuadBallCharacter::SendChallenge);
	PlayerInputComponent->BindAction("Reject", IE_Pressed, this, &AQuadBallCharacter::RejectChallenge);

	PlayerInputComponent->BindAction("KickR", IE_Released, this, &AQuadBallCharacter::KickRight);
	PlayerInputComponent->BindAction("KickL", IE_Released, this, &AQuadBallCharacter::KickLeft);
	PlayerInputComponent->BindAction("Head", IE_Pressed, this, &AQuadBallCharacter::Head);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AQuadBallCharacter::Interact);

	PlayerInputComponent->BindAxis("Right", this, &AQuadBallCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Up", this, &AQuadBallCharacter::MoveUp);
	PlayerInputComponent->BindAxis("AimR", this, &AQuadBallCharacter::AimRight);
	PlayerInputComponent->BindAxis("AimU", this, &AQuadBallCharacter::AimUp);

}

#pragma endregion

#pragma region Settings

bool AQuadBallCharacter::Server_SetName_Validate(const FString &inN) { return true; }

void AQuadBallCharacter::Server_SetName_Implementation(const FString &inN)
{
	playerName = inN;
}

void AQuadBallCharacter::Client_SetMusic_Implementation(USoundCue* musC, bool on)
{
	if (!IsLocallyControlled() || SelfIsAI) return;

	if (on)
	{
		if (musC)
		{
			musicComp->SetSound(musC);
			musicComp->Play();
		}
	}
	else
	{
		musicComp->Stop();
	}

}

// Change music volume, saves updated value to the game instance
void AQuadBallCharacter::ChangeMusicVolume_Implementation(const float percent)
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI || !gameInstance) return;
	
	gameInstance->SetMUSVol(percent);
}

// Change the master volume, saves updated value to the game instance
void AQuadBallCharacter::ChangeMasterVolume_Implementation(const float percent)
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI || !gameInstance) return;
	gameInstance->SetMasterVol(percent);
}

// Change the sfx volume, saves updated value to the game instance
void AQuadBallCharacter::ChangeSFXVolume_Implementation(const float percent)
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI || !gameInstance) return;
	gameInstance->SetSFXVol(percent);
}

// Udpate all volme values
void AQuadBallCharacter::UpdateAllVolume()
{
	if (!IsLocallyControlled()) return;

	if (SelfIsAI) return;
	ChangeMusicVolume(gameInstance->defaultMusicVol);
	ChangeMasterVolume(gameInstance->defaultMasterVol);
	ChangeSFXVolume(gameInstance->defaultSFXVol);
}

void AQuadBallCharacter::SetAnimInstanceClass(TSubclassOf<UQuadBallCharacterAnimInstance> inClass)
{
	animInstanceSourceClass = inClass;
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AQuadBallCharacter::SetMenuAnimInstance()
{
	//if (!menuAnimInstanceSourceClass) return;
	GetMesh()->SetAnimInstanceClass(menuAnimInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AQuadBallCharacter::SetQuadBallAnimInstance()
{
	GetMesh()->SetAnimInstanceClass(animInstanceSourceClass);
	characterAnimInstance = Cast<UQuadBallCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void AQuadBallCharacter::SetCanDispose(bool in)
{
	canDispose = in;
}

// Pause game (local player 1 only)
void AQuadBallCharacter::Pause()
{
	if (!IsLocallyControlled()) return;
	if (isPaused)
	{
		UIWidget->HidePause();
		isPaused = false;
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		APlayerController* menuController = GetWorld()->GetFirstPlayerController();
		menuController->bShowMouseCursor = false;
		menuController->bEnableClickEvents = false;
		menuController->bEnableMouseOverEvents = false;
	}
	else
	{
		UIWidget->ShowPause();
		isPaused = true;
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
		APlayerController* menuController = GetWorld()->GetFirstPlayerController();
		menuController->bShowMouseCursor = true;
		menuController->bEnableClickEvents = true;
		menuController->bEnableMouseOverEvents = true;
	}
}

// Increment debug display level
void AQuadBallCharacter::DebugUp()
{
	if (!HasAuthority()) return;

	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (activeCourt) debugStatus = activeCourt->IncrementDebugStatus();
}

#pragma endregion

FString NetRoleToString(ENetRole role)
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

void AQuadBallCharacter::Debug_PrintString(const int inval, const int key, const int time, const FString append)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString(Role) + ":" + append + FString::FromInt(inval));
}

void AQuadBallCharacter::Debug_PrintString(const float inval, const int key, const int time, const FString append)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString(Role) + ":" + append + FString::SanitizeFloat(inval));
}

void AQuadBallCharacter::Debug_PrintString(const FString &inval, const int key, const int time, const FString append)
{
	GEngine->AddOnScreenDebugMessage(key, time, FColor::Cyan, NetRoleToString(Role) + ":" + append + inval);
}

void AQuadBallCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AQuadBallCharacter, isExecutingSpecialMove);
	DOREPLIFETIME(AQuadBallCharacter, specialMoveToExecute);
	DOREPLIFETIME(AQuadBallCharacter, isSpecialKicking);
	DOREPLIFETIME(AQuadBallCharacter, currentATB);
	DOREPLIFETIME(AQuadBallCharacter, isBoosting);
	DOREPLIFETIME(AQuadBallCharacter, isKicking);
	DOREPLIFETIME(AQuadBallCharacter, ballInteractCooldownRemain);
	DOREPLIFETIME(AQuadBallCharacter, chargeAmount);
	DOREPLIFETIME(AQuadBallCharacter, gameMode);
	DOREPLIFETIME(AQuadBallCharacter, ackServe);
	DOREPLIFETIME(AQuadBallCharacter, kickingKey);
	DOREPLIFETIME(AQuadBallCharacter, commitAimX);
	DOREPLIFETIME(AQuadBallCharacter, commitAimY);
	DOREPLIFETIME(AQuadBallCharacter, isPlaying);
	DOREPLIFETIME(AQuadBallCharacter, currentSpecialMove);
	DOREPLIFETIME(AQuadBallCharacter, isSpiking);
	DOREPLIFETIME(AQuadBallCharacter, hasKickedRight);
	DOREPLIFETIME(AQuadBallCharacter, hasKickedLeft);
	DOREPLIFETIME(AQuadBallCharacter, hasHandstandRight);
	DOREPLIFETIME(AQuadBallCharacter, hasHandstandLeft);
	DOREPLIFETIME(AQuadBallCharacter, lastJuggle);
	DOREPLIFETIME(AQuadBallCharacter, lastKickChanceUsed);
	DOREPLIFETIME(AQuadBallCharacter, lastHandChanceUsed);
	DOREPLIFETIME(AQuadBallCharacter, playerID);
	DOREPLIFETIME(AQuadBallCharacter, isCharging);
	DOREPLIFETIME(AQuadBallCharacter, handStand);
	DOREPLIFETIME(AQuadBallCharacter, canControlMove);
	DOREPLIFETIME(AQuadBallCharacter, playerName);
	DOREPLIFETIME(AQuadBallCharacter, OpponentIsAI);
	DOREPLIFETIME(AQuadBallCharacter, challengeRequestActive);	
	DOREPLIFETIME(AQuadBallCharacter, sender);
	DOREPLIFETIME(AQuadBallCharacter, challengeResponseTimerActive);
	DOREPLIFETIME(AQuadBallCharacter, canStartChallengeGame);
	DOREPLIFETIME(AQuadBallCharacter, isSwimming);

}

// Called every frame
void AQuadBallCharacter::Tick(float DeltaTime)
{
	if (!IsLocallyControlled()) return;
	Super::Tick(DeltaTime);

	if (startTryWait > 0)
	{
		startTryWait -= DeltaTime;
	}

	if (gameMode != QUAD_BALL_GAME_MODE) return;

	Server_SetATB(DeltaTime);
	Server_SetBallCooldown(DeltaTime);
	Server_SetChargingValue(DeltaTime);
	if (isBoosting) Server_CommandBoostMove(DeltaTime);
	if (isExecutingSpecialMove) Server_CommandBallSpecialMove(DeltaTime);

	// Handle case of ATB dropping while special is active, cancel it
	if (isSpecialKicking && currentATB < maxATB)
	{
		isSpecialKicking = false;
		MULTICAST_CancelSpecialActiveParticles();
	}

	// Handle kicking logic
	if (isKicking && activeBall->GetBallPlayStatus() != 0) 
	{
		Server_SetAim(aimX, aimY, commitAimX, commitAimY, myCourt);
		Server_ValidateBallKick();
	}


	WorldUIWidget->UpdateChargeBar(minCharge, maxCharge, chargeAmount);
	WorldUIWidget->UpdateATB(currentATB, maxATB);
	if (isCharging)
	{
		playerAimerComp->SetWorldLocation(activeCourt->GetPositionOnCourt(playerID, commitAimX, commitAimY, myCourt));
		WorldUIWidget->SetAimerLocation(playerID, commitAimX, commitAimY, myCourt);
		WorldUIWidget->SetSpinAimerLocation(spinDeltaX / spinMaxX, spinDeltaY / spinMaxY, myCourt);
	}
	else
	{
		playerAimerComp->SetWorldLocation(activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt));
		WorldUIWidget->SetAimerLocation(playerID, aimX, aimY, myCourt);
	}

	WorldUIWidget->SetMoveType(currentSpecialMove);
	WorldUIWidget->ShowPlayerResponsibiliy(activeBall->GetPlayerBallResponsibility() == playerID ? true : false);
}

#pragma region Animation actions and particle systems and sound

void AQuadBallCharacter::MULTICAST_KickAnim_Implementation(const int kickKey)
{
	Debug_PrintString("Depricated", -1, 99);
}

// Handle reset animation only
void AQuadBallCharacter::PlayBallReset()
{
	characterAnimInstance->PlayBallReset();
}

// Not implemented
void AQuadBallCharacter::CycleAnimation()
{
	return;
	//interactAnimInstance->Cycle();
}

// Execute the hit voice effect
void AQuadBallCharacter::MULTICAST_DoHitVoice_Implementation()
{
	float voicePlayF = FMath::FRandRange(0, 10);

	if (voicePlayF > 3)
	{
		voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_HIT_PARAM);
		voiceComp->Play();
	}
}

// Execute the hit voice effect
void AQuadBallCharacter::MULTICAST_DoDesiredVoice_Implementation(const float param)
{
	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, param);
	voiceComp->Play();
}


void AQuadBallCharacter::PlayStartVoice()
{
	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_GAMESTART_PARAM);
	voiceComp->Play();
}

#pragma endregion

#pragma region Context animations

void AQuadBallCharacter::PlayMenuIdle()
{
	characterAnimInstance->PlayMenuIdle();
}

void AQuadBallCharacter::PlayMenuCharacterSelect()
{
	characterAnimInstance->PlayMenuCharacterSelect();
}

bool AQuadBallCharacter::PlayBeginLevelAnim_Validate()
{
	return true;
}

void AQuadBallCharacter::PlayBeginLevelAnim_Implementation()
{
	canControlMove = true;
	GetMesh()->SetHiddenInGame(false);
}

// Play win animation, severe indicates if it is a round or game win
bool AQuadBallCharacter::Server_PlayWin_Validate(const int severe) { return true; }

void AQuadBallCharacter::Server_PlayWin_Implementation(const int severe)
{
	canControlMove = false;
	MULTICAST_PlayWin(severe);
}

void AQuadBallCharacter::MULTICAST_PlayWin_Implementation(int severe)
{
	if (severe)
	{
		characterOnceEffectComponent = UGameplayStatics::SpawnEmitterAttached(gameWinEffect, GetMesh(), FName("Impact"));
		characterOnceEffectComponent->bAutoDestroy = true;
		voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_END_PARAM);
		voiceComp->Play();
	}

	characterAnimInstance->PlayWin(severe);

}

// Enable movement after win animation, called from BP
bool AQuadBallCharacter::Server_EndWinAnim_Validate() { return true; }

void AQuadBallCharacter::Server_EndWinAnim_Implementation()
{
	canControlMove = true;
}

void AQuadBallCharacter::PlayMenuStartAnimation()
{
	characterAnimInstance->PlayMenuAnimSpawn();
}

void AQuadBallCharacter::PlayMenuEndAnimation()
{
	characterAnimInstance->PlayMenuAnimDestroy();
}

#pragma endregion

#pragma region Handstand

// Transition movement mode from standard to handstand
void AQuadBallCharacter::HandstandModeTransition()
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	Server_RequestHandstand();
}

bool AQuadBallCharacter::Server_RequestHandstand_Validate() { return true; }

void AQuadBallCharacter::Server_RequestHandstand_Implementation()
{
	if (handStand)
	{
		handStand = false;
	}
	else
	{
		handStand = true;
	}
	MULTICAST_Handstand(handStand);
}

void AQuadBallCharacter::MULTICAST_Handstand_Implementation(bool on)
{
	if (on)	characterAnimInstance->HandstandOn();
	else characterAnimInstance->HandstandOff();
}

#pragma endregion

#pragma region Movement

// Allow character to move, called from BP
void AQuadBallCharacter::SetAllowMove(const bool in)
{
	canControlMove = in;
}

// Right movement logic, QuadBall: Control strafe, World: Rotate player 
void AQuadBallCharacter::MoveRight(const float axisVal)
{
	if ((gameMode != IDLE_GAME_MODE && gameMode != QUAD_BALL_GAME_MODE) || !canControlMove) return;
	//axisVal = axisVal * localPlayerID;
	if ((Controller != NULL) && (axisVal != 0.0f) && gameMode == QUAD_BALL_GAME_MODE)
	{
		FVector direction = GetCapsuleComponent()->GetRightVector();
		AddMovementInput(direction * axisVal * (1 - (.6f * int(isCharging)) - (.2f * int(handStand))), 1, false);
		//GetMesh()->AddRelativeLocation(direction * axisVal * 10);
	}
	else if ((Controller != NULL) && (axisVal != 0.0f) && gameMode == IDLE_GAME_MODE)
	{
		// find out which way is right
		FVector direction = GetCapsuleComponent()->GetRightVector();
		AddControllerYawInput(rotateSpeed * axisVal * GetWorld()->GetDeltaSeconds());
		//GetMesh()->AddRelativeLocation(direction * axisVal * 10);
	}
}

// Up Movement logic, moves character forward or back regardless of game mode
void AQuadBallCharacter::MoveUp(const float axisVal)
{
	if ((gameMode !=IDLE_GAME_MODE) && (gameMode != QUAD_BALL_GAME_MODE) || !canControlMove) return;
	//axisVal = axisVal * localPlayerID;
	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		FVector direction = GetCapsuleComponent()->GetForwardVector();
		AddMovementInput(direction * axisVal * (1 - (.5 * isSwimming)) * (1 - (.6f * int(isCharging)) - (.2f * int(handStand))), 1, false);
	}
}

bool AQuadBallCharacter::Server_SetSwimming_Validate(bool swim) { return true; }

void AQuadBallCharacter::Server_SetSwimming_Implementation(bool swim)
{
	isSwimming = swim;
	MULTICAST_SetSwimming(swim);
}

void AQuadBallCharacter::MULTICAST_SetSwimming_Implementation(bool swim)
{
	characterAnimInstance->SetSwimming(swim);
}

bool AQuadBallCharacter::Server_SetAim_Validate(float aX, float aY, float cAX, float cAY, int mC) { return true; }

void AQuadBallCharacter::Server_SetAim_Implementation(float aX, float aY, float cAX, float cAY, int mC)
{
	aimX = aX;
	aimY = aY;
	commitAimX = cAX;
	commitAimY = cAY;
	myCourt = mC;
}

void AQuadBallCharacter::Client_GetAim_Implementation()
{
	Server_SetAim(aimX, aimY, commitAimX, commitAimY, myCourt);
}

// Right aim logic, QuadBall: Change aim location, World: Rotate camera
void AQuadBallCharacter::AimRight(float axisVal)
{
	if (isPaused) return;
	if (gameMode != QUAD_BALL_GAME_MODE)
	{
		if ((Controller != NULL) && (axisVal != 0.0f))
		{
			playerCameraBoom->AddWorldRotation(FRotator(0, 1.5 * axisVal, 0));
		}
	}
	else
	{
		if (playerID == 1) axisVal *= -1;
		if ((Controller != NULL) && (axisVal != 0.0f))
		{
			float val = FMath::Abs(axisVal);

			if (axisVal > 0)
			{
				if (isCharging)
				{
					if (spinDeltaX < spinMaxX)
					{
						spinDeltaX += val / 10;
					}
				}
				else
				{
					if (aimX <= 1)
					{
						aimX += val / 50;
					}
				}
			}
			else if (axisVal < 0)
			{
				if (isCharging)
				{
					if (spinDeltaX > -spinMaxX)
					{
						spinDeltaX -= val / 10;
					}
				}
				else
				{
					if (aimX >= -1)
					{
						aimX -= val / 50;
					}
				}
			}

			spinDeltaX > spinMaxX ? spinDeltaX = spinMaxX : NULL;
			spinDeltaX < -spinMaxX ? spinDeltaX = -spinMaxX : NULL;
			aimX > 1 ? aimX = 1 : NULL;
			aimX < -1 ? aimX = -1 : NULL;
		}
	}
}

// Up aim logic, QuadBall: Change aim location, World: Rotate camera
void AQuadBallCharacter::AimUp(const float axisVal)
{
	//axisVal = axisVal * localPlayerID;
	if (gameMode != QUAD_BALL_GAME_MODE) return;

	if ((Controller != NULL) && (axisVal != 0.0f))
	{
		float val = FMath::Abs(axisVal);
		if (axisVal < 0)
		{
			if (isCharging)
			{
				if (spinDeltaY > -spinMaxY)
				{
					spinDeltaY -= val / 10;
				}
			}
			else
			{
				if (aimY > 0)
				{
					aimY -= val / 50;
				}
			}
		}
		else if (axisVal > 0)
		{
			if (isCharging)
			{
				if (spinDeltaY < spinMaxY)
				{
					spinDeltaY += val / 10;
				}
			}
			else
			{
				if (aimY >= 0)
				{
					aimY += val / 50;
				}
			}
		}

		spinDeltaY > spinMaxY ? spinDeltaY = spinMaxY : NULL;
		spinDeltaY < -spinMaxY ? spinDeltaY = -spinMaxY : NULL;
		aimY > 1 ? aimY = 1 : NULL;
		aimY < 0 ? aimY = 0 : NULL;
	}
}

// Jump
void AQuadBallCharacter::DoJump()
{
	Jump();
}

#pragma endregion

#pragma region challenge

void AQuadBallCharacter::SendChallenge()
{
	if (!IsLocallyControlled()) return;

	if (canStartChallengeGame && startTryWait <= 0)
	{
		if (startTryWait <= 0)
		{
			Server_RequestStartChallengeGame(this, challengedPlayer);
			startTryWait = startTryWaitMax;
		}
		else
		{
			if (!SelfIsAI)
			{
				UIWidget->CourtNotFoundError();
			}
		}
		return;
	}

	if (canChallenge && challengedPlayer && !challengeRequestActive)
	{
		if (!SelfIsAI)
		{
			Server_SendChallengeRequest(challengedPlayer);
		}
	}
	else if (challengeRequestActive && sender)
	{
		if (!SelfIsAI)
		{
			Server_RevokeChallengeRequest(challengedPlayer);
			canChallenge = false;
			UIWidget->CanChallenge(false);
		}
	}
	else if (challengeRequestActive && !sender)
	{
		if (!SelfIsAI)
		{
			UIWidget->ChallengeAccepted();
		}
		Server_AckChallengeRequest(challengedPlayer);
	}
}

void AQuadBallCharacter::RejectChallenge()
{
	if (!IsLocallyControlled() || !challengeRequestActive) return;

	if ((challengeRequestActive && !sender) || canStartChallengeGame)
	{
		Server_RevokeChallengeRequest(challengedPlayer);
		canChallenge = false;
		UIWidget->CanChallenge(false);
	}
}

bool AQuadBallCharacter::Server_SendChallengeRequest_Validate(AQuadBallCharacter* otherPlayer) { return true; }

void AQuadBallCharacter::Server_SendChallengeRequest_Implementation(AQuadBallCharacter* otherPlayer)
{
	challengeRequestActive = true;
	sender = true;
	Client_AckChallengeSent();

	MULTICAST_DoDesiredVoice(VOICE_GAMESTART_PARAM);

	if (otherPlayer)
	{
		otherPlayer->SetChallengeRequestActive(true);
		otherPlayer->SetIsSender(false);
		otherPlayer->Client_RecieveChallenge(this);
		if (otherPlayer->GetSelfIsAI())
		{
			otherPlayer->SetChallengeResponseTimerActive(true);
		}
	}
}

bool AQuadBallCharacter::Server_AckChallengeRequest_Validate(AQuadBallCharacter* otherPlayer) { return true; }

void AQuadBallCharacter::Server_AckChallengeRequest_Implementation(AQuadBallCharacter* otherPlayer)
{
	canStartChallengeGame = true;
	MULTICAST_DoDesiredVoice(VOICE_GAMESTART_PARAM);
	if (otherPlayer)
	{
		otherPlayer->SetCanStartChallengeGame(true);
		if (!otherPlayer->GetSelfIsAI())
		{
			otherPlayer->Client_AckChallengeAccepted();
		}
		else
		{
			//Debug_PrintString(FString("Attempting to start game from ") + otherPlayer->GetName() + " with " + this->GetName(), -1, 50);
			otherPlayer->Server_RequestStartChallengeGame(otherPlayer, this);
		}
	}
	else
	{
		//Debug_PrintString("No other player", -1, 10);
	}
}

bool AQuadBallCharacter::Server_RevokeChallengeRequest_Validate(AQuadBallCharacter* otherPlayer) { return true; }

void AQuadBallCharacter::Server_RevokeChallengeRequest_Implementation(AQuadBallCharacter* otherPlayer)
{
	challengeRequestActive = false;
	canStartChallengeGame = false;
	if (otherPlayer)
	{
		otherPlayer->SetCanStartChallengeGame(false);
		otherPlayer->SetChallengeRequestActive(false);
		otherPlayer->Client_RevokeChallenge(this);
	}
}

void AQuadBallCharacter::Client_AckChallengeSent_Implementation()
{
	if (!IsLocallyControlled() || SelfIsAI) return;
	UIWidget->ChallengeSent(true, challengedPlayer);

}

void AQuadBallCharacter::Client_AckChallengeAccepted_Implementation()
{
	if (!IsLocallyControlled() || SelfIsAI) return;
	UIWidget->ChallengeAccepted();
}

void AQuadBallCharacter::Client_NoCourtAvailable_Implementation()
{
	if (!IsLocallyControlled() || SelfIsAI) return;
	UIWidget->CourtNotFoundError();
}

void AQuadBallCharacter::Client_RecieveChallenge_Implementation(AQuadBallCharacter* otherPlayer)
{
	if (!IsLocallyControlled()) return;
	challengedPlayer = otherPlayer;
	//Debug_PrintString(GetName() + FString(" recieved challenge from ") + otherPlayer->GetName(), -1, 50);
	if (!SelfIsAI)
	{
		UIWidget->ChallengeRec(true, otherPlayer);
	}
}

void AQuadBallCharacter::Client_RevokeChallenge_Implementation(AQuadBallCharacter* otherPlayer)
{
	if (!IsLocallyControlled()) return;
	canChallenge = false;

	if (!SelfIsAI)
	{
		UIWidget->ChallengeRec(false, otherPlayer);
	}
}

bool AQuadBallCharacter::Server_RequestStartChallengeGame_Validate(class AQuadBallCharacter* player1, class AQuadBallCharacter* player2) { return true; }

void AQuadBallCharacter::Server_RequestStartChallengeGame_Implementation(class AQuadBallCharacter* player1, class AQuadBallCharacter* player2)
{
	if (!player1 || !player2) return;

	AQuadBallCourt* foundCourt = FetchAvailableCourt();

	if (!foundCourt)
	{
		player1->Client_NoCourtAvailable();
		player2->Client_NoCourtAvailable();
	}
	else
	{
		
		player1->SetCanStartChallengeGame(false);
		player2->SetCanStartChallengeGame(false);
		foundCourt->Server_QueForGame(player1, true);
		foundCourt->Server_QueForGame(player2, true);
	}
}

AQuadBallCourt* AQuadBallCharacter::FetchAvailableCourt()
{
	TArray<AActor*> aArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuadBallCourt::StaticClass(), aArr);

	if (aArr.Num() != 0)
	{
		for (auto i : aArr)
		{
			AQuadBallCourt* t = Cast<AQuadBallCourt>(i);
			if (!t->GetIsInSession() && t->GetPlayersInQue() == 0)
			{
				return t;
			}
		}
	}

	return nullptr;
}

#pragma endregion

#pragma region Quad Ball Initialization
// An overlap of the body block volume causes the BodyBlock() effect, only for active ball.
void AQuadBallCharacter::OnOverlapBeginBodyBlock(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (ballInteractCooldownRemain == 0 && !(activeBall->GetPlayerBallResponsibility() == playerID && activeBall->GetIsServe()))
	{
		BodyBlock();
	}
}

// Configure player collision for quad ball boundaires, on activates collision, !on deactivates it
void AQuadBallCharacter::SetGameCollision(const bool on)
{
	if (on) GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);
	else GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
}

bool AQuadBallCharacter::Server_EndGame_Validate() { return true; }

void AQuadBallCharacter::Server_EndGame_Implementation()
{
	if (activeCourt) activeCourt->Server_EndMatch();
}

// Ends a quad ball game prematurely
void AQuadBallCharacter::EndQuadBallGame()
{
	Server_EndGame();
}

// Ends a quad ball game naturally
void AQuadBallCharacter::EndQuadBallGameByPoints()
{
	//if (localPlayerID == 1) UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(Cast<ACameraActor>(playerCamera));
	bUseControllerRotationYaw = true;
	playerCameraBoom->TargetArmLength = standardCameraDist;
	playerCameraBoom->SetRelativeRotation(FRotator(cameraPitchNormal, 0, 0));
	Client_UI_HideGameUI();
	playerAimerComp->SetHiddenInGame(true);

	if (isCharging)
	{
		isCharging = false;
		MULTICAST_CancelChargeParticles();
	}

	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_END_PARAM);
	voiceComp->Play();

	if (isPaused) Pause();
}

#pragma endregion

#pragma region interact

// Handles player interaction with world objects. Overlapping a QuadBall game start volume prompts the user to queue for a quad ball game.  Overlapping with a world interactable object provides interaction capabilities
void AQuadBallCharacter::OnCapsuleOverlapInteractableBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (gameMode != IDLE_GAME_MODE || SelfIsAI) return;

	if (OtherActor->IsA(AQuadBallCourt::StaticClass()) && !challengeRequestActive)
	{
		AQuadBallCourt* c = Cast<AQuadBallCourt>(OtherActor);
		if (!c) return;

		if (c->GetIsInSession()) return;
		activeCourt = Cast<AQuadBallCourt>(OtherActor);
		activeBall = activeCourt->GetActiveBall();
		interactCode = 1;

		activeCourt->LookAt(this, true);

		if (IsLocallyControlled())
		{
			UIWidget->ShowPlayQuadBallUI(interactCode);
		}

	}
	else if (OtherActor->GetClass() == AWorldInteractable::StaticClass())
	{
		AWorldInteractable* other = Cast<AWorldInteractable>(OtherActor);
		if (!other) return;

		if (other->GetUsed()) return;

		interactableObject = other;
		interactCode = interactableObject->GetInteractType();

		if (IsLocallyControlled())
		{
			UIWidget->ShowPlayQuadBallUI(interactCode);
		}

	}
	else if (OtherActor->IsA(AQuadBallCharacter::StaticClass()))
	{
		if (!IsLocallyControlled() || challengeRequestActive) return;

		challengedPlayer = Cast<AQuadBallCharacter>(OtherActor);

		if (challengedPlayer->GetChallengeRequestActive() || challengedPlayer->GetIsPlaying())
		{
			challengedPlayer = nullptr;
			canChallenge = false;
			return;
		}

		canChallenge = true;
		UIWidget->CanChallenge(true);
	}
	else if (OtherActor->IsA(ATriggerVolume::StaticClass()))
	{
		if (!IsLocallyControlled()) return;
		Server_SetSwimming(true);
		characterAnimInstance->SetSwimming(true);
	}
}

// Handle effects after loss of focus of interactable objects (QuadBall courts or world interactables)
void AQuadBallCharacter::OnCapsuleOverlapInteractableEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (gameMode != IDLE_GAME_MODE || SelfIsAI) return;
	interactCode = -1;

	if (otherActor->IsA(AQuadBallCourt::StaticClass()))
	{
		if (activeCourt) activeCourt->LookAt(this, false);
	}
	else if (otherActor->IsA(AQuadBallCharacter::StaticClass()))
	{
		if (IsLocallyControlled())
		{
			if (!challengeRequestActive)
			{
				canChallenge = false;
				UIWidget->CanChallenge(false);
			}
		}
	}
	else if (otherActor->IsA(ATriggerVolume::StaticClass()))
	{
		if (IsLocallyControlled())
		{
			Server_SetSwimming(false);
			characterAnimInstance->SetSwimming(false);
		}
	}

	if (IsLocallyControlled() && gameMode != INTERACT_GAME_MODE)
	{
		UIWidget->HidePlayQuadBallUI();
	}

}

void AQuadBallCharacter::MULTICAST_InteractAnim_Implementation(bool on)
{
	if (on)
	{
		characterAnimInstance->DoInteractAnim(interactCode);
		GetCapsuleComponent()->SetEnableGravity(false);
		GetCharacterMovement()->GravityScale = 0;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);

		FVector pointTowardsDir = (interactableObject->GetExitPoint()->GetComponentLocation() - interactableObject->GetInteractPoint()->GetComponentLocation());
		pointTowardsDir.Z = 0;
		GetMesh()->SetWorldRotation(pointTowardsDir.ToOrientationRotator());

	}
	else
	{
		characterAnimInstance->CancelInteractAnim();
		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->GravityScale = 1;
		GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel6, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel7, ECR_Overlap);
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	}
}

bool AQuadBallCharacter::Server_RequestInteract_Validate(bool on) { return true; }

void AQuadBallCharacter::Server_RequestInteract_Implementation(bool on)
{
	if (on)
	{
		MULTICAST_InteractAnim(true);
		gameMode = INTERACT_GAME_MODE;
		GetCapsuleComponent()->SetWorldLocation(interactableObject->GetInteractPoint()->GetComponentLocation());
		interactableObject->SetUsed(true);
	}
	else
	{
		MULTICAST_InteractAnim(false);

		gameMode = IDLE_GAME_MODE;
		interactableObject->SetUsed(false);
		SetActorLocation(interactableObject->GetExitPoint()->GetComponentLocation());
	}

}

bool AQuadBallCharacter::Server_ForceEndInteract_Validate() { return true; }

void AQuadBallCharacter::Server_ForceEndInteract_Implementation()
{

	MULTICAST_InteractAnim(false);
	Server_SetSwimming(false);
	if (!SelfIsAI && interactableObject) interactableObject->SetUsed(false);

}

// Handles interact logic
void AQuadBallCharacter::Interact()
{
	if (!IsLocallyControlled()) return;
	if (interactCode == -1) return;

	// Quad ball court interaction
	if (interactCode == COURT_INTERACT_CODE && gameMode == IDLE_GAME_MODE)
	{
		Server_RequestQueForGame();
		interactCode = -1;
	}
	// Interact with world interactable object, interact code defines the type of interaction
	else if (interactCode > INTERACTABLE_INTERACT_CODE)
	{
		if (!interactableObject) return;

		// Not currently interacting
		if (gameMode == IDLE_GAME_MODE)
		{
			if (interactableObject->GetUsed()) return;
			Server_RequestInteract(true);
		}
		else if (gameMode == INTERACT_GAME_MODE) // Currently interacting
		{
			Server_RequestInteract(false);
		}
	}
}

#pragma endregion

#pragma region charge

bool AQuadBallCharacter::Server_Charge_Validate(const int key) { return true; }

void AQuadBallCharacter::Server_Charge_Implementation(const int key)
{
	if (!isCharging)
	{
		isCharging = true;
		isAscending = true;
		chargeAmount = minCharge;
		MULTICAST_PlayChargeParticles(key);
	}
}

bool AQuadBallCharacter::Server_CancelCharge_Validate() { return true; }

void AQuadBallCharacter::Server_CancelCharge_Implementation()
{
	isCharging = false;
	spinDeltaX = 0;
	spinDeltaY = 0;
	MULTICAST_CancelChargeParticles();
}

bool AQuadBallCharacter::Server_SetChargingValue_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_SetChargingValue_Implementation(float DeltaTime)
{
	// Handle charging or aiming update
	if (isCharging && !isPaused)
	{
		//UIWidget->SetSpinAimerLocation(spinDeltaX / spinMaxX, spinDeltaY / spinMaxY);
		if (chargeAmount < maxCharge && isAscending)
		{
			chargeAmount += chargeMult * DeltaTime;
			if (chargeAmount >= maxCharge)
			{
				isAscending = false;
				chargeAmount = maxCharge;
			}
		}
		else if (chargeAmount > minCharge && !isAscending)
		{
			chargeAmount -= chargeMult * DeltaTime;
			if (chargeAmount <= minCharge)
			{
				isAscending = true;
				chargeAmount = minCharge;
			}
		}
	}
}

void AQuadBallCharacter::OnRep_chargeAmount()
{
	if (!IsLocallyControlled() || !WorldUIWidget) return;
	WorldUIWidget->UpdateChargeBar(minCharge, maxCharge, chargeAmount);
}

// Activate charge effect
void AQuadBallCharacter::Charge(const int key)
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	commitAimX = aimX;
	commitAimY = aimY;
	Server_Charge(key);
}

// Cancel charge effect
void AQuadBallCharacter::CancelCharge()
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	Server_CancelCharge();
}

// Deactivate charge active particles if currently exists, no effect if not currently active
void AQuadBallCharacter::MULTICAST_CancelChargeParticles_Implementation()
{
	if (chargeParticleComponent)
	{
		chargeParticleComponent->DestroyComponent();
		isPlayingChargeParticles = false;
	}
}

// Play particle system for charging, loops, does not self delete
void AQuadBallCharacter::MULTICAST_PlayChargeParticles_Implementation(const int key)
{
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (!isPlayingChargeParticles)
	{
		if (chargeParticleComponent) chargeParticleComponent->DestroyComponent();
		if (key == 1) chargeParticleComponent = UGameplayStatics::SpawnEmitterAttached(chargingEffect, GetMesh(), FName("Foot_L"));
		else if (key == 2) chargeParticleComponent = UGameplayStatics::SpawnEmitterAttached(chargingEffect, GetMesh(), FName("Foot_R"));
		else if (key == 5) chargeParticleComponent = UGameplayStatics::SpawnEmitterAttached(chargingEffect, GetMesh(), FName("FX_Head"));

		isPlayingChargeParticles = true;
	}
}

// Play a particle system once, must not loop, plays at "Impact" socket, self deletes
void AQuadBallCharacter::MULTICAST_PlayCharacterOnceEffect_Implementation(UParticleSystem* inSys)
{

	characterOnceEffectComponent = UGameplayStatics::SpawnEmitterAttached(inSys, GetMesh(), FName("Impact"));
	characterOnceEffectComponent->bAutoDestroy = true;

}

#pragma endregion

#pragma region que and enter game

bool AQuadBallCharacter::Server_RequestQueForGame_Validate() { return true; }

void AQuadBallCharacter::Server_RequestQueForGame_Implementation()
{
	Client_ToggleQ(true);
	activeCourt->Server_QueForGame(this);
}

// Setup player values for QuadBall play
bool AQuadBallCharacter::Server_InitPlayerForQuadBall_Validate(const int pID, AQuadBallBall* ball, AQuadBallCourt* court) { return true; }

void AQuadBallCharacter::Server_InitPlayerForQuadBall_Implementation(const int pID, class AQuadBallBall* ball, class AQuadBallCourt* court)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);
}

bool AQuadBallCharacter::Server_InitPlayerForIdle_Validate() { return true; }

void AQuadBallCharacter::Server_InitPlayerForIdle_Implementation()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);
}

void AQuadBallCharacter::Client_InitPlayerForQuadBall_Implementation(class AQuadBallBall* ball, class AQuadBallCourt* court)
{
	if (!IsLocallyControlled()) return;

	Server_ForceEndInteract();

	if (ball == nullptr || court == nullptr)
	{
		Debug_PrintString("Null court or ball", -1, 99);
		return;
	}

	TArray<AActor*> aArr;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("WORLD_MUSIC"), aArr);

	if (aArr.Num() > 0)
	{
		AAmbientSound* musSound = CastChecked<AAmbientSound>(aArr[0]);
		if (musSound)
		{
			musSound->Play();
		}
	}

	canStartChallengeGame = false;
	canChallenge = false;
	UIWidget->CanChallenge(false);

	activeBall = ball;
	activeCourt = court;

	Server_InitPlayerForQuadBall(0, nullptr, nullptr);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);

	playerCameraBoom->TargetArmLength = quadBallCameraDist;
	playerCameraBoom->SetRelativeRotation(FRotator(cameraPitchQuadBall, 0, 0));
	playerAimerComp->SetHiddenInGame(false);

	UpdateAllVolume();
	activeBall = activeCourt->GetActiveBall();
	Client_UI_ClearHistory();
	Client_UI_ShowGameUI();
	Client_ToggleQ(false);
}

void AQuadBallCharacter::Client_InitPlayerForIdle_Implementation()
{
	bUseControllerRotationYaw = true;
	playerCameraBoom->TargetArmLength = standardCameraDist;
	playerCameraBoom->SetRelativeRotation(FRotator(cameraPitchNormal, 0, 0));
	playerAimerComp->SetHiddenInGame(true);
	if (IsLocallyControlled())
	{
		UIWidget->HideUI_QuadBall();
	}

	if (isCharging)
	{
		isCharging = false;
		MULTICAST_CancelChargeParticles();
	}

	Server_InitPlayerForIdle();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Ignore);

	if (isPaused) Pause();
}

void AQuadBallCharacter::MULTICAST_InitPlayerLocation_Implementation()
{
	characterAnimInstance->NeedToRefresh(true);
	//
}

bool AQuadBallCharacter::Server_SetInitQuadBallOrientation_Validate(const FVector i, const FRotator r) { return true; }

// Configure player orientation for quad ball play
void AQuadBallCharacter::Server_SetInitQuadBallOrientation_Implementation(const FVector i, const FRotator r)
{
	SetActorLocation(i);
	Controller->ClientSetRotation(r);
	MULTICAST_InitPlayerLocation();
}

void AQuadBallCharacter::Client_SetInitQuadBallOrientation_Implementation(const FVector i, const FRotator r)
{

}

#pragma endregion

#pragma region ATB

bool AQuadBallCharacter::Server_SetATB_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_SetATB_Implementation(float DeltaTime)
{
	if (currentATB < maxATB && activeBall->GetBallPlayStatus() != 0 && !isPaused)
	{
		currentATB += ((DeltaTime / timeToFillATB) / maxATB);
		if (currentATB > maxATB) currentATB = maxATB;
	}
}

void AQuadBallCharacter::OnRep_currentATB()
{
	if (!IsLocallyControlled() || !WorldUIWidget) return;
	WorldUIWidget->UpdateATB(currentATB, maxATB);
}

#pragma endregion

bool AQuadBallCharacter::Server_SetBallCooldown_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_SetBallCooldown_Implementation(float DeltaTime)
{
	// Prevent multi-frame succesive ball interactions
	if (ballInteractCooldownRemain > 0)
	{
		ballInteractCooldownRemain -= GetWorld()->GetDeltaSeconds();
	}
	if (ballInteractCooldownRemain < 0) ballInteractCooldownRemain = 0;
}

#pragma region Kick

bool AQuadBallCharacter::Server_RequestKickLeft_Validate() { return true; }

void AQuadBallCharacter::Server_RequestKickLeft_Implementation()
{
	ackServe = true;
	if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetReadyToServe() && !isBoosting)
	{
		if (activeBall->GetPlayerBallResponsibility() == playerID)
		{
			DoServe(true);
		}
	}
	else
	{
		if (currentSpecialMove != 0)
		{
			isSpecialKicking = true;
		}
		else
		{
			isSpecialKicking = false;
		}
		MULTICAST_KickLeft();
	}
	isCharging = false;
}

bool AQuadBallCharacter::Server_RequestKickRight_Validate() { return true; }

void AQuadBallCharacter::Server_RequestKickRight_Implementation()
{
	ackServe = true;
	if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetReadyToServe() && !isBoosting)
	{
		if (activeBall->GetPlayerBallResponsibility() == playerID)
		{
			DoServe(true);
		}
	}
	else
	{
		if (currentSpecialMove != 0)
		{
			isSpecialKicking = true;
		}
		else
		{
			isSpecialKicking = false;
		}
		MULTICAST_KickRight();
	}
	isCharging = false;
}

void AQuadBallCharacter::MULTICAST_KickLeft_Implementation()
{
	if (Role != ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->LeftKick(currentSpecialMove);
	}
}

void AQuadBallCharacter::MULTICAST_KickRight_Implementation()
{
	if (Role != ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->RightKick(currentSpecialMove);
	}
}

// Execute right kick action
void AQuadBallCharacter::KickRight()
{
	if (gameMode != QUAD_BALL_GAME_MODE || isPaused) return;

	if (ENetRole::ROLE_AutonomousProxy)
	{
		if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetPlayerBallResponsibility() == playerID)
		{

		}
		else
		{
			characterAnimInstance->RightKick(currentSpecialMove);
		}
	}

	Server_RequestKickRight();
	CancelCharge();
}

// Execute left kick action
void AQuadBallCharacter::KickLeft()
{
	if (gameMode != QUAD_BALL_GAME_MODE || isPaused) return;

	if (Role == ENetRole::ROLE_AutonomousProxy)
	{
		if (activeBall->GetBallPlayStatus() == 0 && activeBall->GetPlayerBallResponsibility() == playerID)
		{

		}
		else
		{
			characterAnimInstance->LeftKick(currentSpecialMove);
		}

	}
	CancelCharge();
	Server_RequestKickLeft();
}

#pragma endregion

#pragma region head and body

void AQuadBallCharacter::MULTICAST_Head_Implementation()
{
	if (Role != ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->Head();
	}
}

bool AQuadBallCharacter::Server_Head_Validate() { return true; }

void AQuadBallCharacter::Server_Head_Implementation()
{
	MULTICAST_Head();
}

// Execute head action
void AQuadBallCharacter::Head()
{
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (handStand) return;

	if (Role == ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->Head();
	}
	Server_Head();
	CancelCharge();
}

#pragma endregion

#pragma region boost

void AQuadBallCharacter::MULTICAST_BoostRight_Implementation()
{
	if (Role != ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->BoostRight();
	}
	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_HIT_PARAM);
	voiceComp->Play();
}

bool AQuadBallCharacter::Server_RequestBoostRight_Validate(float in1, float in2, FVector in3) { return true; }

void AQuadBallCharacter::Server_RequestBoostRight_Implementation(float in1, float in2, FVector in3)
{
	isBoosting = true;
	commitBoostForward = in1;
	commitBoostHoriz = in2;
	commitBoostDir = in3;
	commitBoostDir.Normalize();

	timeInBoost = 0;
	currentATB -= .5;
	boostDir = 1;

	MULTICAST_BoostRight();
}

void AQuadBallCharacter::MULTICAST_BoostLeft_Implementation()
{
	if (Role != ENetRole::ROLE_AutonomousProxy)
	{
		characterAnimInstance->BoostLeft();
	}

	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_HIT_PARAM);
	voiceComp->Play();
}

bool AQuadBallCharacter::Server_RequestBoostLeft_Validate(float in1, float in2, FVector in3) { return true; }

void AQuadBallCharacter::Server_RequestBoostLeft_Implementation(float in1, float in2, FVector in3)
{
	isBoosting = true;
	commitBoostForward = in1;
	commitBoostHoriz = in2;
	commitBoostDir = in3;
	commitBoostDir.Normalize();

	timeInBoost = 0;
	currentATB -= .5;
	boostDir = 1;
	MULTICAST_BoostLeft();
}

// Do boost for right foot in direction of movement input.  if no input assumed right
void AQuadBallCharacter::BoostKickRight()
{
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (activeBall->GetBallPlayStatus() != 0 && !isPaused && !isBoosting && !handStand && currentATB > .5 && GetCharacterMovement()->IsWalking())
	{
		if (Role == ENetRole::ROLE_AutonomousProxy)
		{
			characterAnimInstance->BoostRight();
		}
		KickProbe(2);

		commitBoostForward = GetInputAxisValue(FName("Up"));
		commitBoostHoriz = GetInputAxisValue(FName("Right"));
		if (FMath::IsNearlyZero(float(commitBoostForward + commitBoostHoriz), 0.1f)) commitBoostHoriz = -1.f;

		commitBoostDir = (GetCapsuleComponent()->GetForwardVector() * commitBoostForward) + (GetCapsuleComponent()->GetRightVector() * commitBoostHoriz);
		commitBoostDir.Normalize();

		Server_RequestBoostRight(commitBoostForward, commitBoostHoriz, commitBoostDir);
	}
}

// Do boost for left foot, in direction of movement input.  if no input assumed left
void AQuadBallCharacter::BoostKickLeft()
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	if (activeBall->GetBallPlayStatus() != 0 && !isPaused && !isBoosting && !handStand && currentATB > .5 && GetCharacterMovement()->IsWalking())
	{
		if (Role == ENetRole::ROLE_AutonomousProxy)
		{
			characterAnimInstance->BoostLeft();
		}

		KickProbe(1);

		commitBoostForward = GetInputAxisValue(FName("Up"));
		commitBoostHoriz = GetInputAxisValue(FName("Right"));
		if (FMath::IsNearlyZero(float(commitBoostForward + commitBoostHoriz), 0.1f)) commitBoostHoriz = -1.f;
		commitBoostDir = (GetCapsuleComponent()->GetForwardVector() * commitBoostForward) + (GetCapsuleComponent()->GetRightVector() * commitBoostHoriz);
		commitBoostDir.Normalize();

		Server_RequestBoostLeft(commitBoostForward, commitBoostHoriz, commitBoostDir);
	}
}

// Cancel boost action
bool AQuadBallCharacter::Server_CancelBoost_Validate() { return true; }

void AQuadBallCharacter::Server_CancelBoost_Implementation()
{
	isBoosting = false;
	isKicking = false;
	MULTICAST_CancelBoost();
}

void AQuadBallCharacter::MULTICAST_CancelBoost_Implementation()
{
	characterAnimInstance->CancelBoost();
}

bool AQuadBallCharacter::Server_CommandBoostMove_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_CommandBoostMove_Implementation(float DeltaTime)
{
	timeInBoost += DeltaTime;
	if (boostKickAccelCurve)
	{
		float minT, maxT;
		boostKickAccelCurve->GetTimeRange(minT, maxT);

		if (timeInBoost < maxT)
		{
			FVector out = (commitBoostDir * boostVel * boostKickAccelCurve->GetFloatValue(timeInBoost));
			AddActorWorldOffset(out, false, nullptr, ETeleportType::None);
		}
		else
		{
			Server_CancelBoost();
		}
	}
}

#pragma endregion

#pragma region special moves

FVector AQuadBallCharacter::GetPathMidpoint()
{
	FVector charLoc = GetCapsuleComponent()->GetComponentLocation();
	FVector aimLoc = activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt);
	FVector netLoc = activeCourt->GetCourtNet()->GetComponentLocation() + FVector(0, 0, 50);

	FVector aDir = aimLoc - charLoc;
	aDir.Normalize();
	FVector normal = activeCourt->GetActorForwardVector();
	normal.Normalize();
	if (playerID == 0) 	normal *= -1;

	float d = FVector::DotProduct(netLoc, normal);
	float denom = FVector::DotProduct(aDir, normal);
	if (FMath::IsNearlyZero(denom)) return FVector::ZeroVector;
	float t = -(d + FVector::DotProduct(charLoc, normal) / denom);
	if (FMath::IsNearlyZero(t)) return FVector::ZeroVector;

	return charLoc + aDir * t;
}

void AQuadBallCharacter::DrawSpecialMovePath()
{
	FVector charLoc = GetCapsuleComponent()->GetComponentLocation();
	FVector aimLoc = activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt);
	FVector netLoc = activeCourt->GetCourtNet()->GetComponentLocation() + FVector(0, 0, 50);

	FVector aDir = aimLoc - charLoc;
	aDir.Normalize();

	FVector midPoint = GetPathMidpoint();

	FVector aDir2 = aimLoc - midPoint;
	aDir2.Normalize();

	if (midPoint == FVector::ZeroVector)
	{
		Debug_PrintString("Bad Result");
		return;
	}

	midPoint.Z = activeCourt->GetMinimumSafeZLocation().Z + 100;
	DrawDebugBox(GetWorld(), midPoint, FVector(10, 10, 10), FColor::Emerald);

	float distToMid = (midPoint - charLoc).Size();
	float distMidToTarget = (aimLoc - midPoint).Size();
	float phase1Dist = 0;
	float phase2Dist = 0;

	for (phase1Dist; phase1Dist < distToMid; phase1Dist += (distToMid / 100))
	{
		float percent = (phase1Dist / distToMid) / 2;
		FVector thisStep = charLoc + (phase1Dist *  aDir);
		thisStep.Z = characterSpecialCurve1->GetVectorValue(percent).Z * midPoint.Z;

		DrawDebugSphere(GetWorld(), thisStep, 2, 3, FColor::Emerald);
	}

	for (phase2Dist; phase2Dist < distMidToTarget; phase2Dist += (distMidToTarget / 100))
	{
		float percent = (phase2Dist / distMidToTarget) / 2;
		FVector thisStep = midPoint + (phase2Dist *  aDir2);
		thisStep.Z = characterSpecialCurve1->GetVectorValue(.5 + percent).Z * midPoint.Z;

		DrawDebugSphere(GetWorld(), thisStep, 2, 3, FColor::Orange);
	}
}

// Attempt to assign active special move, requires ATB maxed
void AQuadBallCharacter::SetSpecialMove(const int move)
{
	if (!IsLocallyControlled()) return;

	if (currentATB != 2) return;
	Server_SetSpecialMove(move);

}

bool AQuadBallCharacter::Server_SetSpecialMove_Validate(const int move) { return true; }

void AQuadBallCharacter::Server_SetSpecialMove_Implementation(const int move)
{
	if (currentSpecialMove == move)
	{
		currentSpecialMove = 0;
		MULTICAST_CancelSpecialActiveParticles();
	}
	else
	{
		currentSpecialMove = move;
		MULTICAST_PlaySpecialActiveParticles();
	}
}

// Control ball movement for special move
bool AQuadBallCharacter::Server_CommandBallSpecialMove_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_CommandBallSpecialMove_Implementation(float DeltaTime)
{
	if (isExecutingSpecialMove)
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



				thisStepH = thisStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent).Y);
				nextStepH = nextStepH + ballRight * (characterSpecialCurve1->GetVectorValue(percent2).Y);

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

bool AQuadBallCharacter::Server_SpecialMoveInit_Validate() { return true; }

void AQuadBallCharacter::Server_SpecialMoveInit_Implementation()
{
	specialMoveToExecute = currentSpecialMove;
	currentSpecialMove = 0;
	isSpecialKicking = false;
	isExecutingSpecialMove = true;

	currentATB = 0;

	MULTICAST_CancelSpecialActiveParticles();
	MULTICAST_DoHitVoice();

	if (specialMoveToExecute == 1) Server_DoSpecialKick1(GetWorld()->GetDeltaSeconds());
	else if (specialMoveToExecute == 2) Server_DoSpecialKick2(GetWorld()->GetDeltaSeconds());
}

bool AQuadBallCharacter::Server_DoSpecialKick1_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_DoSpecialKick1_Implementation(float DeltaTime)
{
	Client_GetAim();
	specialMidPoint = GetPathMidpoint();

	if (specialMidPoint == FVector::ZeroVector)
	{
		isExecutingSpecialMove = false;
		Server_DoBallKick(.5);
	}

	specialMidPoint.Z = activeCourt->GetMinimumSafeZLocation().Z + 100;

	activeBall->MULTICAST_SetIsSpecialDriven(true);
	MULTICAST_PlayCharacterOnceEffect(specialPlayerBurst);

	specialCharLoc = GetCapsuleComponent()->GetComponentLocation();
	specialAimLoc = activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt);
	specialNetLoc = activeCourt->GetCourtNet()->GetComponentLocation() + FVector(0, 0, 50);

	specialElapsedDist = 0;
	specialPhase1Dist = 0;
	specialPhase2Dist = 0;

	specialDirToMid = specialAimLoc - specialCharLoc;
	specialDirToMid.Normalize();
	specialDirToEnd = specialAimLoc - specialMidPoint;
	specialDirToEnd.Normalize();

	specialDistToMid = (specialMidPoint - specialCharLoc).Size();
	specialDistToEnd = (specialAimLoc - specialMidPoint).Size();

	Server_CommandBallSpecialMove(DeltaTime);
}

bool AQuadBallCharacter::Server_DoSpecialKick2_Validate(float DeltaTime) { return true; }

void AQuadBallCharacter::Server_DoSpecialKick2_Implementation(float DeltaTime)
{
	Client_GetAim();
	specialMidPoint = GetPathMidpoint();

	if (specialMidPoint == FVector::ZeroVector)
	{
		isExecutingSpecialMove = false;
		Server_DoBallKick(.5);
	}

	specialMidPoint.Z = activeCourt->GetMinimumSafeZLocation().Z + 100;

	activeBall->MULTICAST_SetIsSpecialDriven(true);
	MULTICAST_PlayCharacterOnceEffect(specialPlayerBurst);

	specialCharLoc = GetCapsuleComponent()->GetComponentLocation();
	specialAimLoc = activeCourt->GetPositionOnCourt(playerID, aimX, aimY, myCourt);
	specialNetLoc = activeCourt->GetCourtNet()->GetComponentLocation() + FVector(0, 0, 50);

	specialElapsedDist = 0;
	specialPhase1Dist = 0;
	specialPhase2Dist = 0;

	specialDirToMid = specialAimLoc - specialCharLoc;
	specialDirToMid.Normalize();
	specialDirToEnd = specialAimLoc - specialMidPoint;
	specialDirToEnd.Normalize();

	specialDistToMid = (specialMidPoint - specialCharLoc).Size();
	specialDistToEnd = (specialAimLoc - specialMidPoint).Size();

	Server_CommandBallSpecialMove(DeltaTime);
}

bool AQuadBallCharacter::Server_SetIsExecSpecial_Validate(bool in) { return true; }

void AQuadBallCharacter::Server_SetIsExecSpecial_Implementation(bool in)
{
	isExecutingSpecialMove = in;
}

// Play particle system for special active, loops, does not self delete
void AQuadBallCharacter::MULTICAST_PlaySpecialActiveParticles_Implementation()
{
	if (specialActiveComponent) specialActiveComponent->DestroyComponent();
	specialActiveComponent = UGameplayStatics::SpawnEmitterAttached(specialActiveEffect, GetMesh(), FName("Impact"));

	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_SPECIAL_PARAM);
	voiceComp->Play();
}

// Deactivate special active particles if currently exist, no effect if not currently active
void AQuadBallCharacter::MULTICAST_CancelSpecialActiveParticles_Implementation()
{
	if (specialActiveComponent)
	{
		specialActiveComponent->DestroyComponent();
	}
}

#pragma endregion

#pragma region ball effect

bool AQuadBallCharacter::Server_SetClean_Validate(bool clean) { return true; }

void AQuadBallCharacter::Server_SetClean_Implementation(bool clean)
{
	activeBall->Server_SetClean(clean);
}

bool AQuadBallCharacter::Server_SetJuggle_Validate(bool juggle) { return true; }

void AQuadBallCharacter::Server_SetJuggle_Implementation(bool juggle)
{
	activeBall->SetJuggleStatus(juggle);
}

bool AQuadBallCharacter::Server_ValidateBallKick_Validate() { return true; }

void AQuadBallCharacter::Server_ValidateBallKick_Implementation()
{
	goodForKick = GetBallInInteractRange(kickingKey);
	if (goodForKick)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, FString("GoodForKick: ") + FString::FromInt(kickingKey));
		if (playerID != activeBall->GetPlayerBallResponsibility())
		{
			activeBall->SetPlayerBallResponsibility(playerID, 0, true);
			activeBall->SetBallPlayStatus(1);
		}

		if (activeBall->GetBallPlayStatus() == 1 && kickingKey != 5)
		{
			activeBall->SetBallBounceCount(0);
		}

		if (kickingKey != 5) activeBall->SetBallPlayStatus(2);

		if (activeBall->GetJuggleStatus())
		{
			if (lastJuggle != kickingKey || kickingKey == 5)
			{
				lastJuggle = kickingKey;
				Client_UI_AddCardToHistory(kickingKey, true, false);

			}
			else
			{
				activeBall->SetBallPlayStatus(0);
				Server_AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2, "Double Juggle!");
				Client_UI_AddCardToHistory(kickingKey, true, true);
			}
		}
		else
		{
			if ((kickingKey == 2 && hasKickedRight) || (kickingKey == 1 && hasKickedLeft) || (kickingKey == 3 && hasHandstandLeft) || (kickingKey == 4 && hasHandstandRight))
			{
				activeBall->SetBallPlayStatus(0);
				Server_AwardPoint((activeBall->GetPlayerBallResponsibility() + 1) % 2, "Double Kick!");
				Client_UI_AddCardToHistory(kickingKey, false, true);
			}
			else
			{
				if (kickingKey == 1) hasKickedLeft = true;
				if (kickingKey == 2) hasKickedRight = true;
				if (kickingKey == 3) hasHandstandLeft = true;
				if (kickingKey == 4) hasHandstandRight = true;
				//WorldUIWidget->ClearCard(kickingKey);
				lastJuggle = kickingKey;
				Client_UI_AddCardToHistory(kickingKey, false, false);
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
			Server_SetClean(false);
		}

		DoBallKick(chargeAmount);
		activeBall->UpdateBallBounceCount(activeBall->GetBallBounceCount());
		Server_SetJuggle(true);
		KickProbe(-1);
	}
}

bool AQuadBallCharacter::Server_DoBallKick_Validate(const float VM) { return true; }

void AQuadBallCharacter::Server_DoBallKick_Implementation(const float vM)
{
	// Only applicable for quad ball gameplay, and special kick must not already be active (else accidental double kicks are possible)
	if (gameMode != QUAD_BALL_GAME_MODE || isExecutingSpecialMove) return;

	// If character is in air and kick key is "head", spike
	if (!GetCharacterMovement()->IsWalking() && kickingKey == 5 && currentATB > 1)
	{
		isSpiking = true;
		currentSpecialMove = 0;
		FVector courtAimPos = activeCourt->GetPositionOnCourt(playerID, commitAimX, commitAimY, myCourt);
		FVector delta = courtAimPos - activeBall->GetBallLocation();
		delta.Normalize();

		currentATB -= (maxATB / 2);

		MULTICAST_DoHitVoice();

		activeBall->Server_DoBallAction(delta * kickVelocity);
		return;
	}
	else if (isSpecialKicking && (kickingKey == 1 || kickingKey == 2)) // Handle special action
	{
		Server_SpecialMoveInit();
		return;
	}

	isSpiking = false;

	if (activeBall->GetIsSpecialDriven())
	{
		activeBall->MULTICAST_SetIsSpecialDriven(false);
	}

	//Debug_PrintString(commitAimX);
	//Debug_PrintString(commitAimY);

	//Debug_PrintString("Setting shit " + FString::SanitizeFloat(aimX) + "," + FString::SanitizeFloat(aimY) + "," + FString::SanitizeFloat(commitAimX) + "," + FString::SanitizeFloat(commitAimY), -1, 10);
	FVector courtAimPos = activeCourt->GetPositionOnCourt(playerID, commitAimX, commitAimY, myCourt);
	FVector delta = courtAimPos - activeBall->GetBallLocation();
	// ZX plane
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
		if (v >= kickVelocity * 4) temp = .5;
	}
	float angleRad = FMath::Atan2((v*v) + temp, g*x);
	delta.Normalize();

	FVector d = (delta * FVector(FMath::Cos(angleRad), FMath::Cos(angleRad), 0)) + FVector(0, 0, FMath::Sin(angleRad));
	d.Normalize();

	FVector dir = d * v * vM;

	FVector spinDir = GetMesh()->GetForwardVector() * spinDeltaX + GetMesh()->GetRightVector() * spinDeltaY;
	activeBall->Server_DoBallAction(dir);

	effectComp->Play();
	MULTICAST_DoHitVoice();

	activeBall->GetBallMesh()->SetPhysicsAngularVelocityInDegrees(spinDir * 2000);
	ballInteractCooldownRemain = ballInteractCooldownMax;

	if (!recievedKickBonus) currentATB += kickBonusToAtb;
	recievedKickBonus = true;
	if (currentATB > maxATB) currentATB = maxATB;
}

#pragma endregion

#pragma region reset

bool AQuadBallCharacter::Server_ResetFromPlayer_Validate() { return true; }

void AQuadBallCharacter::Server_ResetFromPlayer_Implementation()
{
	if (gameMode == QUAD_BALL_GAME_MODE)
	{
		if (activeBall->GetBallPlayStatus() != 0) return;
		if (OpponentIsAI || (activeBall->GetPlayerBallResponsibility() == playerID && activeBall->GetIsServe()) || (activeBall->GetPlayerBallResponsibility() != playerID && !activeBall->GetIsServe()))
		{
			if (!activeBall->GetNeedsToBeReset()) return;
			{
				ackServe = false;
				activeBall->Server_RequestBallReset();

				if (gameMode == IDLE_GAME_MODE) return;

				switch (activeBall->GetPlayerBallResponsibility())
				{
				case 0:
					activeCourt->GetPlayerA()->MULTICAST_Reset();
					break;
				case 1:
					activeCourt->GetPlayerB()->MULTICAST_Reset();
					break;
				default:
					break;
				}
			}
		}
	}
}

void AQuadBallCharacter::MULTICAST_Reset_Implementation()
{
	characterAnimInstance->PlayBallReset();
	voiceComp->SetFloatParameter(VOICE_PARAM_NAME, VOICE_RESET_PARAM);
	voiceComp->Play();
}

void AQuadBallCharacter::ResetFromPlayer()
{
	if (gameMode == QUAD_BALL_GAME_MODE)
	{
		Server_ResetFromPlayer();
	}
	else
	{
		playerCameraBoom->SetRelativeRotation(FRotator(playerCameraBoom->GetRelativeTransform().Rotator().Pitch, 0, playerCameraBoom->GetRelativeTransform().Rotator().Roll));
	}
}

#pragma endregion

#pragma region serve

bool AQuadBallCharacter::Server_DoServe_Validate() { return true; }

void AQuadBallCharacter::Server_DoServe_Implementation()
{
	activeBall->Server_RequestServe();
}

void AQuadBallCharacter::MULTICAST_Serve_Implementation()
{
	characterAnimInstance->Serve(false);
}

// Play serve animation DoServe()->DoExecServe()
void AQuadBallCharacter::DoServe(const bool fast)
{
	MULTICAST_Serve();
}

#pragma endregion

#pragma region Quad Ball Main Actions
// Change aim target from opponents court to one's one court
void AQuadBallCharacter::AlternateTargetCourtSide()
{
	if (gameMode != QUAD_BALL_GAME_MODE) return;
	myCourt = myCourt * -1;
}

void AQuadBallCharacter::InformPlayerOfBounce()
{

}

// Handle ball location reset only called from BP
void AQuadBallCharacter::BallResetLocation() const
{

}

// Returns true if the ball is within interact range from the specified limb
bool AQuadBallCharacter::GetBallInInteractRange(int limbKey)
{
	if (gameMode != QUAD_BALL_GAME_MODE || limbKey > 5 || limbKey < 1) return false;

	ballInInteractRange = false;

	FVector dist = activeBall->GetBallLocation() - GetMesh()->GetSocketLocation(SocketNames[limbKey-1]);
	if (dist.Size() <= maxInteractDist) ballInInteractRange = true;

	return ballInInteractRange;
}

// Start kick search for specified limb
void AQuadBallCharacter::KickProbe(const int key)
{
	if (key > 0 && key < 9)
	{
		isKicking = true;
		kickingKey = key;
	}
	else
	{
		goodForKick = false;
		isKicking = false;
		kickingKey = -1;
		isSpecialKicking = false;
		//CancelCharge();
	}
}

// Execute serve action on ball
void AQuadBallCharacter::DoExecServe()
{

}

// Execute body block action, acts as a "kick" action but is exempt from standard kick "probe" action.  Executed immediately
void AQuadBallCharacter::BodyBlock()
{
	if (!IsLocallyControlled()) return;
	if (gameMode != QUAD_BALL_GAME_MODE) return;

	if (activeBall->GetBallPlayStatus() == 0 || isExecutingSpecialMove || isSpiking || isSpecialKicking) return;
	// Ball is not the current player's responsibility so must change it to be so
	// Also must set ball back to 2 bounce status
	if (playerID != activeBall->GetPlayerBallResponsibility())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Blue, FString("Updating Responsibility"));
		activeBall->SetPlayerBallResponsibility(playerID, 0, true);
		activeBall->SetBallPlayStatus(1);
	}

	// If the Ball is being hit from the opposite side, it is not clean until it passes back over
	if (activeCourt->GetBallSide(activeBall) != playerID && !activeCourt->GetBallSafeDistanceFromNet())
	{
		Server_SetClean(false);
	}

	//UIWidget->ClearCard(6);
	WorldUIWidget->AddCardToHistory(6, false, false);
	UIWidget->AddCardToHistory(6, false, false);
	lastJuggle = 0;
	DoBallKick(bodyMult);
	Server_SetJuggle(true);
	activeBall->UpdateBallBounceCount(activeBall->GetBallBounceCount());
}

bool AQuadBallCharacter::Server_AwardPoint_Validate(const int playerTarget, const FString &reason) { return true; }

void AQuadBallCharacter::Server_AwardPoint_Implementation(const int playerTarget, const FString &reason)
{
	activeCourt->Server_AwardPoint(playerTarget, reason);
}

// Do effect on ball with velocity multiplier vM
void AQuadBallCharacter::DoBallKick(const float vM)
{
	Server_DoBallKick(vM);
}

// Reset the used flags for all kicks due to change in responsibility or point/game reset
bool AQuadBallCharacter::Server_ResetAllKicks_Validate() { return true; }

void AQuadBallCharacter::Server_ResetAllKicks_Implementation()
{
	Client_UI_RefreshAllCards();
	isSpiking = false;
	hasKickedRight = false;
	hasKickedLeft = false;
	hasHandstandRight = false;
	hasHandstandLeft = false;
	lastJuggle = -1;
	lastKickChanceUsed = false;
	lastHandChanceUsed = false;
	recievedKickBonus = false;
}

#pragma endregion

#pragma region Deprecated

// Deprecated
void AQuadBallCharacter::OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

// Deprecated
void AQuadBallCharacter::OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Deprecated
void AQuadBallCharacter::OnOverlapBeginBallHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

// Deprecated
void AQuadBallCharacter::OnOverlapEndBallHead(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


// Deprecated
void AQuadBallCharacter::OnOverlapEndBallBodyBlock(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

// Deprecated
void AQuadBallCharacter::ShowPointResult(FString n, FString rest)
{

}

// Deprecated
void AQuadBallCharacter::HidePointResult()
{

}

// Deprecated
void AQuadBallCharacter::StartQuadBallGame(const bool vsAI)
{

}

// Deprecated
void AQuadBallCharacter::InitLocalPlayer2()
{

}

// Deprecated
void AQuadBallCharacter::UI_SetVSAI(const bool ai)
{

}

// Deprecated
void AQuadBallCharacter::UI_SetPlayers()
{

}

// Deprecated
void AQuadBallCharacter::UI_ShowVSAnim()
{

}

// Deprecated
void AQuadBallCharacter::UI_ChangePlayerResponsible(ACharacter* charToShow)
{

}

#pragma endregion

#pragma region UI

void AQuadBallCharacter::Client_NoOtherPlayers_Implementation()
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI) return;
	UIWidget->NoOtherPlayerError();
}

// Updates the world ui valid action display
void AQuadBallCharacter::Client_UI_RefreshAllCards_Implementation()
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI) return;
	WorldUIWidget->RefreshAllCards();
}

// Clears the action historu from the main UI
void AQuadBallCharacter::Client_UI_ClearHistory_Implementation()
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI) return;
	UIWidget->ClearHistory();
	WorldUIWidget->ClearHistory();
}

// Displas the quad ball UI when game is active
void AQuadBallCharacter::Client_UI_ShowGameUI_Implementation()
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI) return;

	WorldUIWidget->ClearHistory();
	WorldUIWidget->UpdateATB(currentATB, maxATB);
	WorldUIWidget->UpdateChargeBar(minCharge, maxCharge, chargeAmount);
	UIWidget->HidePlayQuadBallUI();

	UIWidget->ShowUI_QuadBall();
	WorldUIWidget->ShowUI_QuadBall();
}

// Hides the quad ball UI when game is finished
void AQuadBallCharacter::Client_UI_HideGameUI_Implementation()
{
	if (!IsLocallyControlled()) return;
	if (SelfIsAI) return;

	UIWidget->HideUI_QuadBall();
	WorldUIWidget->HideUI_QuadBall();
}

void AQuadBallCharacter::Client_UI_AddCardToHistory_Implementation(int a, bool b, bool c)
{
	if (!IsLocallyControlled()) return;
	WorldUIWidget->AddCardToHistory(a, b, c);
	UIWidget->AddCardToHistory(a, b, c);
}

void AQuadBallCharacter::Client_ToggleQ_Implementation(bool on)
{
	if (!IsLocallyControlled()) return;
	UIWidget->ToogleQ(on);
}

#pragma endregion
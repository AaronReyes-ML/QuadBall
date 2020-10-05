// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadBallMenuPawn.h"
#include "Camera/CameraComponent.h"
#include "QuadBallMenuUI.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "QuadBallCharacter.h"
#include "Yin.h"
#include "Shimbi.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "QuadBallGameInstance.h"
#include "Components/CapsuleComponent.h"
#include "QuadBallCharacterAnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "QuadBallGameModeBase.h"
#include "Engine.h"


// Sets default values
AQuadBallMenuPawn::AQuadBallMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	rootC = CreateDefaultSubobject<USceneComponent>("Rooot");
	SetRootComponent(rootC);

	// Create an orthographic camera (no perspective) and attach it to the boom
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));

	menuAmbientComponent = CreateDefaultSubobject<UAudioComponent>("Menu Ambient");
	menuMusicComponent = CreateDefaultSubobject<UAudioComponent>("Menu Music");

	static ConstructorHelpers::FObjectFinder<USoundCue> cue1(TEXT("/Game/Audio/Music/MenuMusic_Cue"));
	static ConstructorHelpers::FObjectFinder<USoundCue> cue2(TEXT("/Game/Audio/SFX/ocean1_Cue"));

	static ConstructorHelpers::FObjectFinder<USoundClass> sCl1(TEXT("SoundClass'/Game/Audio/SFX.SFX'"));
	SFX_SoundClass = Cast<USoundClass>(sCl1.Object);
	static ConstructorHelpers::FObjectFinder<USoundClass> sCl2(TEXT("SoundClass'/Game/Audio/New_MSTR.New_MSTR'"));
	MSTR_SoundClass = Cast<USoundClass>(sCl2.Object);
	static ConstructorHelpers::FObjectFinder<USoundClass> sCl3(TEXT("SoundClass'/Game/Audio/MUSIC.MUSIC'"));
	MUS_SoundClass = Cast<USoundClass>(sCl3.Object);


	menuMusicCue = cue1.Object;
	menuAmbientCue = cue2.Object;

	menuAmbientComponent->SetSound(menuAmbientCue);
	menuMusicComponent->SetSound(menuMusicCue);

	p1Transform = CreateDefaultSubobject<USceneComponent>("P1 Transform");
	p1Transform->SetupAttachment(RootComponent);
	p2Transform = CreateDefaultSubobject<USceneComponent>("P2 Transform");
	p2Transform->SetupAttachment(RootComponent);
	pointToDirectionTransform = CreateDefaultSubobject<USceneComponent>("Point to direction transform");
	p1TransformCharaChange = CreateDefaultSubobject<USceneComponent>("P1 T CharaChange");
	p1TransformCharaChange->SetupAttachment(RootComponent);
	p2TransformCharaChange = CreateDefaultSubobject<USceneComponent>("P2 T CharaChange");
	p2TransformCharaChange->SetupAttachment(RootComponent);

	cameraTransformStandard = CreateDefaultSubobject<USceneComponent>("Cam Transform Stand");
	cameraTransformStandard->SetupAttachment(RootComponent);
	cameraTransformOptions = CreateDefaultSubobject<USceneComponent>("Cam Transform Options");
	cameraTransformOptions->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh1(TEXT("/Engine/BasicShapes/Plane"));
	p1SplashPlane = CreateDefaultSubobject<UStaticMeshComponent>("p1 splash");
	p1SplashPlane->SetupAttachment(RootComponent);
	p1SplashPlane->SetStaticMesh(mesh1.Object);
	p2SplashPlane = CreateDefaultSubobject<UStaticMeshComponent>("p2 splash");
	p2SplashPlane->SetupAttachment(RootComponent);
	p2SplashPlane->SetStaticMesh(mesh1.Object);

}

// Called when the game starts or when spawned
void AQuadBallMenuPawn::BeginPlay()
{
	Super::BeginPlay();

	menuMusicComponent->Stop();
	UIWidget = Cast<UQuadBallMenuUI>(CreateWidget<UUserWidget>(GetWorld(), UIWidgetBase));
	if (UIWidget)
	{
		UIWidget->AddToViewport();
	}
	gameInstance = Cast<UQuadBallGameInstance>(GetGameInstance());

	APlayerController* menuController = GetWorld()->GetFirstPlayerController();
	menuController->bShowMouseCursor = true;
	menuController->bEnableClickEvents = true;
	menuController->bEnableMouseOverEvents = true;

	UIWidget->SetVolumeSliders(gameInstance->defaultSFXVol, gameInstance->defaultMusicVol, gameInstance->defaultMasterVol);

	FVector t = (pointToDirectionTransform->GetComponentLocation() - p1Transform->GetComponentLocation());
	t.Z = 0;
	FVector t2 = (pointToDirectionTransform->GetComponentLocation() - p2Transform->GetComponentLocation());
	t2.Z = 0;

	FActorSpawnParameters newParams = FActorSpawnParameters();
	newParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	p1 = GetWorld()->SpawnActor<AYin>(p1Transform->GetComponentLocation(), t.ToOrientationRotator(), newParams);
	p1->SetMenuAnimInstance();
	p1->SetCanDispose(true);
	p1SplashPlane->SetMaterial(0, (UMaterialInterface*)p1->GetCharacterPortrait());
	p2 = GetWorld()->SpawnActor<AShimbi>(p2Transform->GetComponentLocation(), t2.ToOrientationRotator(), newParams);
	p2->SetMenuAnimInstance();
	p2->SetCanDispose(true);
	p2SplashPlane->SetMaterial(0, (UMaterialInterface*)p2->GetCharacterPortrait());

	p1->PlayMenuIdle();
	p1->GetMesh()->SetHiddenInGame(false);
	p2->PlayMenuIdle();
	p2->GetMesh()->SetHiddenInGame(false);

	UIWidget->SetPlayer1(p1);
	UIWidget->SetPlayer2(p2);
}

void AQuadBallMenuPawn::ChangeMenuMode()
{
	if (menuMode == MAIN_MODE)
	{
		menuMode = OPTION_MODE;
	}
	else
	{
		menuMode = MAIN_MODE;
	}
	modeTransitionActive = true;
}

float AQuadBallMenuPawn::DistComponents(USceneComponent* comp1, USceneComponent* comp2)
{
	if (!comp1 || !comp2) return -1;

	FVector dist = comp2->GetComponentLocation() - comp1->GetComponentLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, FString::SanitizeFloat(dist.Size()));
	return dist.Size();

}

void AQuadBallMenuPawn::InterpCamera(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, "InterpCam");
	if (modeTransitionActive && menuMode == MAIN_MODE)
	{
		if (FMath::IsNearlyZero(DistComponents(camera, cameraTransformStandard), 1.f))
		{
			modeTransitionActive = false;
			//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, "Interp Done");
		}
		else
		{
			camera->SetWorldLocation(FMath::InterpEaseInOut(camera->GetComponentLocation(),cameraTransformStandard->GetComponentLocation(),.1,1));
			camera->SetWorldRotation(FMath::InterpEaseInOut(camera->GetComponentRotation(), cameraTransformStandard->GetComponentRotation(), .1, 1));
		}
	}
	else if (modeTransitionActive && menuMode == OPTION_MODE)
	{
		if (FMath::IsNearlyZero(DistComponents(camera, cameraTransformOptions), .1f))
		{
			modeTransitionActive = false;
			//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Blue, "Interp Done");
		}
		else
		{
			camera->SetWorldLocation(FMath::InterpEaseInOut(camera->GetComponentLocation(), cameraTransformOptions->GetComponentLocation(), .1, 1));
			camera->SetWorldRotation(FMath::InterpEaseInOut(camera->GetComponentRotation(), cameraTransformOptions->GetComponentRotation(), .1, 1));
		}
	}
}

void AQuadBallMenuPawn::MoveCharacters()
{
	//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, "Moving Characters");
	if (menuMode == MAIN_MODE)
	{
		if (p1 && p2)
		{
			SetupPlayerState(true);
			p1->SetActorLocation(p1Transform->GetComponentLocation());
			p1->SetActorRotation((pointToDirectionTransform->GetComponentLocation() - p1Transform->GetComponentLocation()).ToOrientationRotator());
			p2->SetActorLocation(p2Transform->GetComponentLocation());
			p2->SetActorRotation((pointToDirectionTransform->GetComponentLocation() - p2Transform->GetComponentLocation()).ToOrientationRotator());
		}
	}
	else
	{
		if (p1 && p2)
		{
			SetupPlayerState(false);
			p1->SetActorLocation(p1TransformCharaChange->GetComponentLocation());
			p1->SetActorRotation((cameraTransformOptions->GetComponentLocation() - p1TransformCharaChange->GetComponentLocation()).ToOrientationRotator());
			p2->SetActorLocation(p2TransformCharaChange->GetComponentLocation());
			p2->SetActorRotation((cameraTransformOptions->GetComponentLocation() - p2TransformCharaChange->GetComponentLocation()).ToOrientationRotator());
		}
	}
}

void AQuadBallMenuPawn::SetupPlayerState(bool idle)
{
	if (p1 && p2)
	{
		if (idle)
		{
			p1->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
			p1->PlayMenuIdle();
			p2->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
			p2->PlayMenuIdle();
		}
		else
		{
			p1->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
			p1->PlayMenuCharacterSelect();
			p2->GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
			p2->PlayMenuCharacterSelect();
		}
	}
}


// Called every frame
void AQuadBallMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (modeTransitionActive)
	{
		InterpCamera(DeltaTime);
	}

}

// Called to bind functionality to input
void AQuadBallMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("StartGame", IE_Pressed, this, &AQuadBallMenuPawn::MenuStartGame);

}

void AQuadBallMenuPawn::ShowMenuUI()
{
	UIWidget->ShowMenuUI();
}

void AQuadBallMenuPawn::MenuStartGame()
{
	if (gameStarted)
	{
		UIWidget->Skip();
		return;
	}
	UIWidget->StarGame();
	menuMusicComponent->Play();
	gameStarted = true;
}

void AQuadBallMenuPawn::SetVolumeLevels(float SFX, float MUSIC, float MASTER)
{
	gameInstance->SetMUSVol(MUSIC);
	gameInstance->SetSFXVol(SFX);
	gameInstance->SetMasterVol(MASTER);
}

AQuadBallCharacter* AQuadBallMenuPawn::InstantPlayer1(UClass* inC)
{
	if (!inC) return nullptr;

	if (p1) p1->Destroy();

	FActorSpawnParameters newParams = FActorSpawnParameters();
	newParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


	FVector t = (pointToDirectionTransform->GetComponentLocation() - p1Transform->GetComponentLocation());

	if (menuMode == MAIN_MODE)
	{
		p1 = (AQuadBallCharacter*)GetWorld()->SpawnActor(inC, &p1Transform->GetComponentTransform(), newParams);
	}
	else
	{
		p1 = (AQuadBallCharacter*)GetWorld()->SpawnActor(inC, &p1TransformCharaChange->GetComponentTransform());
		t = cameraTransformOptions->GetComponentLocation() - p1TransformCharaChange->GetComponentLocation();

	}
	t.Z = 0;
	p1->GetCapsuleComponent()->SetWorldRotation((t).ToOrientationRotator());
	p1->SetMenuAnimInstance();
	p1->GetMesh()->SetHiddenInGame(false);
	p1->PlayMenuStartAnimation();
	p1SplashPlane->SetMaterial(0, (UMaterialInterface*)p1->GetCharacterPortrait());
	p1->PlayStartVoice();

	gameInstance->playerAClass = inC;

	return p1;
}

AQuadBallCharacter* AQuadBallMenuPawn::InstantPlayer2(UClass* inC)
{
	if (!inC) return nullptr;

	if (p2) p2->Destroy();
	FVector t = (pointToDirectionTransform->GetComponentLocation() - p2Transform->GetComponentLocation());

	FActorSpawnParameters newParams = FActorSpawnParameters();
	newParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (menuMode == MAIN_MODE)
	{
		p2 = (AQuadBallCharacter*)GetWorld()->SpawnActor(inC, &p2Transform->GetComponentTransform(), newParams);
		p2->SetMenuAnimInstance();
		SetupPlayerState(true);
	}
	else
	{
		p2 = (AQuadBallCharacter*)GetWorld()->SpawnActor(inC, &p2TransformCharaChange->GetComponentTransform());
		t = cameraTransformOptions->GetComponentLocation() - p2TransformCharaChange->GetComponentLocation();
		p2->SetMenuAnimInstance();
		SetupPlayerState(false);
	}

	t.Z = 0;
	p2->GetCapsuleComponent()->SetWorldRotation((t).ToOrientationRotator());
	p2->PlayMenuStartAnimation();
	p2->GetMesh()->SetHiddenInGame(false);
	p2SplashPlane->SetMaterial(0, (UMaterialInterface*)p2->GetCharacterPortrait());
	p2->PlayStartVoice();

	gameInstance->playerBClass = inC;
	
	return p2;
}


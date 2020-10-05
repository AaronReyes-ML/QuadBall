// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "QuadBallCharacter.generated.h"

UCLASS()
class QUADBALL_API AQuadBallCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQuadBallCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;
	void Debug_PrintString(const int inval, const int key = -1, const int time = 1, const FString append = FString(""));
	void Debug_PrintString(const float inval, const int key = -1, const int time = 1, const FString append = FString(""));
	void Debug_PrintString(const FString &inval, const int key = -1, const int time = 1, const FString append = FString(""));


#pragma region Constants
	const int TWO_BOUNCE_STATUS = 1;
	const int ONE_BOUNCE_STATUS = 2;

	const int IDLE_GAME_MODE = 0;
	const int QUAD_BALL_GAME_MODE = 1;
	const int INTERACT_GAME_MODE = 2;

	const int COURT_INTERACT_CODE = 1;
	const int INTERACTABLE_INTERACT_CODE = 100;

	const FName VOICE_PARAM_NAME = FName("VOICE_TYPE");
	const float VOICE_HIT_PARAM = 0;
	const float VOICE_END_PARAM = 1.1;
	const float VOICE_GAMESTART_PARAM = 2.1;
	const float VOICE_RESET_PARAM = 3.1;
	const float VOICE_SPECIAL_PARAM = 4.1;
#pragma endregion

#pragma region Administrative
	class UQuadBallGameInstance* gameInstance;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UQuadBallCharacterAnimInstance* characterAnimInstance;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UClass* animInstanceSourceClass;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UClass* menuAnimInstanceSourceClass;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UClass* standardAnimInstance;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UMaterial* characterPortrait;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UTexture2D* characterPortraitTexture;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	class AQuadBallCharacter* localPlayer2;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* playerAimerComp;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* playerRotatePoint;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USkeletalMesh* playerSourceMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* playerCamera;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USpringArmComponent* playerCameraBoom;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UBoxComponent* ballInteractBoundBodyBlock;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetName(const FString &inN);

	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float cameraPitchNormal = -5;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float cameraPitchQuadBall = -30;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float standardCameraDist = 200;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float quadBallCameraDist = 700;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	int localPlayerID;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties", Replicated)
	FString playerName = FString("");
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties", Replicated)
	int playerID = -1;
	UPROPERTY(Replicated)
	bool isPlaying = false;
	UPROPERTY(Replicated)
	bool OpponentIsAI = false;
	bool SelfIsAI = false;
	UPROPERTY(Replicated)
	bool ackServe = false;
	bool recievedKickBonus = false;
	UPROPERTY(Replicated)
	int gameMode = 0;
	bool canDispose = true;
#pragma endregion

#pragma region Input and movement
	// Player input and movement
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetAim(float aX, float aY, float cAX, float cAY, int mC);

	void MoveRight(const float axisVal);
	void MoveUp(const float axisVal);
	void AimRight(float axisVal);
	void AimUp(const float axisVal);
	void DoJump();
	void HandstandModeTransition();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CommandBoostMove(float dTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CancelBoost();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_CancelBoost();
	void CycleAnimation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestHandstand();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_Handstand(bool on);

	UPROPERTY(Replicated)
	bool isSwimming = false;
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSwimming(bool swim);
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetSwimming(bool swim);


	UPROPERTY(Replicated)
	bool isBoosting = false;
	UPROPERTY(Replicated)
	bool handStand = false;
	UPROPERTY(Replicated)
	bool canControlMove = true;
	UPROPERTY(Replicated)
	float commitAimX = 0;
	UPROPERTY(Replicated)
	float commitAimY = 0;
	float aimX = .5;
	float aimY = .5;
	float boostDir = 1;
	float commitBoostForward = 0;
	float commitBoostHoriz = 0;
	float timeInBoost = 0;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float rotateSpeed = 45;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* boostKickAccelCurve;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	float boostVel = 500;
	int myCourt = 1;
	FVector commitBoostDir = FVector::ZeroVector;
#pragma endregion

#pragma region Quad Ball
	// Quad Ball Gameplay
	bool GetBallInInteractRange(int limbKey);

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_BoostRight();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestBoostRight(float in1, float in2, FVector in3);
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_BoostLeft();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestBoostLeft(float in1, float in2, FVector in3);

	virtual void BoostKickRight();
	virtual void BoostKickLeft();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AwardPoint(const int playerTarget, const FString &reason);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void Server_DoServe();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_Serve();
	virtual void DoServe(const bool fast);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ResetFromPlayer();


	virtual void ResetFromPlayer();
	void AlternateTargetCourtSide();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestQueForGame();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ValidateBallKick();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestKickLeft();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_KickLeft();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestKickRight();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_KickRight();

	virtual void KickRight();
	virtual void KickLeft();
	virtual void Head();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DoBallKick(const float vM);
	virtual void DoBallKick(const float vM);

	virtual void BodyBlock();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetBallCooldown(float DeltaTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetClean(bool clean);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetJuggle(bool juggle);

	UPROPERTY(Replicated)
	bool isKicking = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_RightKick = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_LeftKick= false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_Head = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_BOOSTL = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_BOOSTR = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_Hand_On = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_Hand_Off = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_Serve = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool REP_Reset = false;


	UPROPERTY(Replicated)
	bool isSpecialKicking = false;
	UPROPERTY(Replicated)
	bool isSpiking = false;
	bool goodForKick = false;
	UPROPERTY(Replicated)
	bool lastKickChanceUsed = false;
	UPROPERTY(Replicated)
	bool lastHandChanceUsed = false;
	UPROPERTY(Replicated)
	bool hasKickedRight = false;
	UPROPERTY(Replicated)
	bool hasKickedLeft = false;
	UPROPERTY(Replicated)
	bool hasHandstandRight = false;
	UPROPERTY(Replicated)
	bool hasHandstandLeft = false;

	bool ballInInteractRange = false;
	bool hasActiveBall = false;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float kickVelocity = 1000;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float standardKickVelocity = 1000;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float velocityMultiplier = 1.0;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float ballInteractCooldownMax = .5;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values", Replicated)
	float ballInteractCooldownRemain = 0;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float headMult = .4;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float kneeMult = .4;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float bodyMult = .2;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float spinMaxX = 1;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float spinMaxY = 1;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float maxInteractDist = 150.f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float headDistMultiplier = .75;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float kneeDistMultiplier = .5;
	float timeInAir = 0;
	float oldTimeInAir = 0;
	float totalDist = 0;
	float spinDeltaX = 0;
	float spinDeltaY = 0;
	UPROPERTY(Replicated)
	int currentSpecialMove = 0;
	UPROPERTY(Replicated)
	int kickingKey = -1;
	UPROPERTY(Replicated)
	int lastJuggle = -1;
	const TArray<FName> SocketNames = {FName("LeftFootSocket"), FName("RightFootSocket"), FName("LeftHand"), FName("RightHand"), FName("HeadTop_EndSocket")};
#pragma endregion

#pragma region Special Moves
	// Special moves

	FVector GetPathMidpoint();
	void DrawSpecialMovePath();

	FVector specialVelocity = FVector::ZeroVector;

	FVector specialCharLoc = FVector::ZeroVector;
	FVector specialAimLoc = FVector::ZeroVector;
	FVector specialNetLoc = FVector::ZeroVector;

	float specialElapsedDist = 0;
	float specialPhase1Dist = 0;
	float specialPhase2Dist = 0;
	FVector specialMidPoint = FVector::ZeroVector;
	float specialDistToMid = 0;
	float specialDistToEnd = 0;
	FVector specialDirToMid = FVector::ZeroVector;
	FVector specialDirToEnd = FVector::ZeroVector;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpecialMoveInit();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSpecialMove(const int key);
	virtual void SetSpecialMove(const int key);
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DoSpecialKick1(float d);
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DoSpecialKick2(float d);
	UFUNCTION(Client, Reliable)
	void Client_GetAim();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_CommandBallSpecialMove(float dSec);

	UPROPERTY(Replicated)
	bool isExecutingSpecialMove = false;
	class UCurveVector* characterSpecialCurve1;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	float specialVel = 1000;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialZCurve;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialXCurve;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialVelCurve;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialZCurve2;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialXCurve2;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UCurveFloat* characterSpecialVelCurve2;
	UPROPERTY(Replicated)
	int specialMoveToExecute = 0;
	FVector vertMove = FVector::ZeroVector;
	FVector horizMove = FVector::ZeroVector;
	FVector aimDir = FVector::ZeroVector;
	FVector rightDir = FVector::ZeroVector;
	FVector totalMove = FVector::ZeroVector;
#pragma endregion

#pragma region ATB and Charge
	// ATB and charge

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Charge(const int key);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_CancelCharge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetATB(float DeltaTime);

	void Charge(const int key);
	void CancelCharge();

	UPROPERTY(EditAnywhere, Category = "Modifyable Values", ReplicatedUsing = OnRep_chargeAmount)
	float chargeAmount = .2f;
	UFUNCTION()
	void OnRep_chargeAmount();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetChargingValue(float DeltaTime);

	UPROPERTY(Replicated)
	bool isCharging = false;
	bool isAscending = true;

	UFUNCTION()
	void OnRep_currentATB();
	UPROPERTY(ReplicatedUsing=OnRep_currentATB)
	float currentATB = 0;

	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float maxATB = 2.f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float atbToExecute = 1.f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float timeToFillATB = 5.f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float kickBonusToAtb = .25;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float minCharge = .5f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float maxCharge = 1.3f;
	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	float chargeMult = 2;
#pragma endregion

#pragma region Overlaps
	// Overlaps
	UFUNCTION()
	void OnOverlapBeginBall(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBall(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnOverlapBeginBallHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBallHead(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnOverlapBeginBodyBlock(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndBallBodyBlock(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnCapsuleOverlapInteractableBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCapsuleOverlapInteractableEnd(UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

#pragma region UI
	// UI
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	TSubclassOf<class UUserWidget> defaultWorldUIClass;
	UPROPERTY(EditAnywhere, Category="Player Unique Properties")
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UQuadBallUI* UIWidget;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UWidgetComponent* inWorldUI;
	class UQuadBallUI* WorldUIWidget;
#pragma endregion

#pragma region Interacts
	// Interact
	UPROPERTY(EditAnywhere, Category = "Components")
	class AWorldInteractable* interactableObject;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestInteract(bool on);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ForceEndInteract();

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_InteractAnim(bool on);

	virtual void Interact();

	int interactCode = -1;
#pragma endregion

#pragma region Debug
	// Debug
	void DebugUp();

	UPROPERTY(EditAnywhere, Category = "Modifyable Values")
	int debugStatus = -1;
#pragma endregion

#pragma region Pause
	// Pause
	void Pause();

	bool isPaused = false;
#pragma endregion

#pragma region challenge

	UPROPERTY(EditAnywhere)
	class USphereComponent* challengeOverlap;

	float autoResponseTime = 1.5;
	float autoResponseTimeMax = 1.5;
	float startTryWait = 5.f;
	float startTryWaitMax = 5.f;

	UPROPERTY(Replicated)
	bool challengeResponseTimerActive = false;
	bool canChallenge = false;
	UPROPERTY(Replicated)
	bool challengeRequestActive = false;
	UPROPERTY(Replicated)
	bool sender = false;
	UPROPERTY(Replicated)
	bool canStartChallengeGame = false;

	void SendChallenge();
	void RejectChallenge();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChallengeRequest(class AQuadBallCharacter* otherPlayer);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RevokeChallengeRequest(class AQuadBallCharacter* otherPlayer);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AckChallengeRequest(class AQuadBallCharacter* otherPlayer);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestStartChallengeGame(class AQuadBallCharacter* player1, class AQuadBallCharacter* player2);

	class AQuadBallCourt* FetchAvailableCourt();

	UFUNCTION(Client, Reliable)
	void Client_AckChallengeSent();
	UFUNCTION(Client, Reliable)
	void Client_RecieveChallenge(class AQuadBallCharacter* otherPlayer);
	UFUNCTION(Client, Reliable)
	void Client_RevokeChallenge(class AQuadBallCharacter* otherPlayer);
	UFUNCTION(Client, Reliable)
	void Client_AckChallengeAccepted();
	UFUNCTION(Client, Reliable)
	void Client_NoCourtAvailable();

	class AQuadBallCharacter* challengedPlayer = nullptr;

#pragma endregion

#pragma region Particles, Sound

	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_DoHitVoice();
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_DoDesiredVoice(const float param);
	void UpdateAllVolume();

	// Particles, sound
	bool isPlayingChargeParticles = false;
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayChargeParticles(const int key);
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_CancelChargeParticles();
	bool isPlayingSpecialActiveParticles = false;
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlaySpecialActiveParticles();
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_CancelSpecialActiveParticles();
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayCharacterOnceEffect(class UParticleSystem* inSys);

	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_KickAnim(const int kickKey);

	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UParticleSystem* chargingEffect;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UParticleSystem* specialActiveEffect;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UParticleSystem* specialBallHitEffect;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UParticleSystem* gameWinEffect;
	UPROPERTY(EditAnywhere, Category = "Player Unique Properties")
	class UParticleSystem* specialPlayerBurst;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UParticleSystemComponent* chargeParticleComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UParticleSystemComponent* specialActiveComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UParticleSystemComponent* characterOnceEffectComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UAudioComponent* voiceComp;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UAudioComponent* effectComp;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UAudioComponent* musicComp;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USoundCue* voiceCue;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USoundCue* effectCue;

#pragma endregion

public:	
	void ShowPointResult(FString p, FString res);
	void HidePointResult();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayStartVoice();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_Reset();

#pragma region Get
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetGameMode() { return gameMode; };
	FORCEINLINE bool GetChallengeRequestActive() { return challengeRequestActive; };
	FORCEINLINE bool GetIsPlaying() { return isPlaying; };
	FORCEINLINE bool GetOpponentIsAi() { return OpponentIsAI; };
	FORCEINLINE bool GetSelfIsAI() { return SelfIsAI; };
	FORCEINLINE bool GetAckServe() { return ackServe; };
	FORCEINLINE bool GetCanControlMove() { return canControlMove; };
	FORCEINLINE bool GetIsExecSpecial() { return isExecutingSpecialMove; };
	FORCEINLINE FString GetQBName() { return playerName; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UMaterial* GetCharacterPortrait() { return characterPortrait; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UTexture2D* GetCharacterPortraitTexture() { return characterPortraitTexture; };
#pragma endregion

#pragma region Set
	FORCEINLINE void SetCanStartChallengeGame(bool s) { canStartChallengeGame = s; };
	FORCEINLINE void SetChallengeResponseTimerActive(bool in) { challengeResponseTimerActive = in; };
	FORCEINLINE void SetIsSender(bool s) { sender = s; };
	FORCEINLINE void SetIsPlaying(const bool in) { isPlaying = in; };
	FORCEINLINE void SetChallengeRequestActive(bool set) { challengeRequestActive = set; };
	FORCEINLINE void SetOpponentIsAI(const bool in) { OpponentIsAI = in; };
	FORCEINLINE void SetSelfIsAI(const bool in) { SelfIsAI = in; };
	FORCEINLINE void SetGameMode(const int o) { gameMode = o; };
	FORCEINLINE void SetActiveBall(class AQuadBallBall* b) { activeBall = b; };
	FORCEINLINE void SetActiveBall(class AQuadBallCourt* c) { activeCourt = c; };
	FORCEINLINE void SetAckServe(const bool inVal) { ackServe = inVal; };
	FORCEINLINE void SetCanControlMove(const bool inVal) { canControlMove = inVal; };
	FORCEINLINE int SetPlayerID(const int id) { return playerID = id; };
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetIsExecSpecial(const bool in);
#pragma endregion

#pragma region Settings Manipulation
	UFUNCTION(Client, Reliable)
	void Client_SetMusic(class USoundCue* musC, bool on);
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ChangeMusicVolume(float percent);
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ChangeMasterVolume(float percent);
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void ChangeSFXVolume(float percent);

	UFUNCTION(BlueprintCallable)
	void SetAnimInstanceClass(TSubclassOf<UQuadBallCharacterAnimInstance> aInst);
	UFUNCTION(BlueprintCallable)
	void SetMenuAnimInstance();
	UFUNCTION(BlueprintCallable)
	void SetQuadBallAnimInstance();
	UFUNCTION(BlueprintCallable)
	void SetCanDispose(bool in);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetCanDispose() { return canDispose; };


#pragma endregion

#pragma region QuadBall Public
	UPROPERTY(EditAnywhere, Category = "Components")
	class AQuadBallBall* activeBall;
	UPROPERTY(EditAnywhere, Category = "Components")
	class AQuadBallCourt* activeCourt;

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_Head();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_Head();


	void PlayMenuIdle();
	void PlayMenuCharacterSelect();

	UFUNCTION(BlueprintCallable)
	virtual void PlayMenuStartAnimation();
	UFUNCTION(BlueprintCallable)
	virtual void PlayMenuEndAnimation();
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_InitPlayerForQuadBall(const int pID, class AQuadBallBall* ball, class AQuadBallCourt* court);
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_InitPlayerForIdle();
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_InitPlayerLocation();
	UFUNCTION(Client, Reliable)
	virtual void Client_InitPlayerForQuadBall(class AQuadBallBall* ball, class AQuadBallCourt* court);
	UFUNCTION(Client, Reliable)
	virtual void Client_InitPlayerForIdle();
	virtual void InformPlayerOfBounce();
	UFUNCTION(BlueprintCallable)
	void StartQuadBallGame(bool vsAI);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndGame();
	UFUNCTION(BlueprintCallable)
	void EndQuadBallGame();
	void EndQuadBallGameByPoints();
	UFUNCTION(BlueprintCallable)
	void DoExecServe();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlayWin(int severe);
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayWin(int severe);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_EndWinAnim();
	void PlayBallReset();
	UFUNCTION(BlueprintCallable)
	void BallResetLocation() const;
	UFUNCTION(BlueprintCallable)
	void SetAllowMove(bool in);
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void PlayBeginLevelAnim();
	void PlayBeginLevelAnim_Implementation();
	void SetGameCollision(bool on);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetInitQuadBallOrientation(const FVector i, const FRotator r);
	UFUNCTION(Client, Reliable)
	void Client_SetInitQuadBallOrientation(const FVector i, const FRotator r);
	UFUNCTION(Client, Reliable)
	void Client_ToggleQ(bool on);

	FRotator last = FRotator::ZeroRotator;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_ResetAllKicks();
	UFUNCTION(BlueprintCallable)
	void KickProbe(const int key);
	void InitLocalPlayer2();
#pragma endregion

#pragma region UI Public
	void UI_SetVSAI(bool AI);
	void UI_SetPlayers();
	void UI_ShowVSAnim();
	void UI_ChangePlayerResponsible(class ACharacter* charToShow);
	UFUNCTION(Client, Unreliable)
	void Client_NoOtherPlayers();
	UFUNCTION(Client, Unreliable)
	void Client_UI_RefreshAllCards();
	UFUNCTION(Client, Unreliable)
	void Client_UI_ClearHistory();
	UFUNCTION(Client, Unreliable)
	void Client_UI_ShowGameUI();
	UFUNCTION(Client, Unreliable)
	void Client_UI_HideGameUI();
	UFUNCTION(Client, Unreliable)
	void Client_UI_AddCardToHistory(int a, bool b, bool c);
#pragma endregion
};

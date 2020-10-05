// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadBallCourt.generated.h"

UCLASS()
class QUADBALL_API AQuadBallCourt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuadBallCourt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UPROPERTY(Replicated)
	FString playerAName = FString("");
	UPROPERTY(Replicated)
	FString playerBName = FString("");

	const FName MAST_VOL = FName("MAST_VOL");
	const FName SFX_VOL = FName("SFX_VOL");
	const FName MUS_VOL = FName("MUS_VOL");

	UPROPERTY(EditAnywhere)
	int pointsToWinRound = 6;
	UPROPERTY(EditAnywhere)
	int pointsToWinGame = 2;

	UPROPERTY(EditAnywhere)
	int gapToWinRound = 6;
	UPROPERTY(EditAnywhere)
	int gapToWinGame = 2;

	UPROPERTY(EditAnywhere)
	float courtWidth = 600.f;
	UPROPERTY(EditAnywhere)
	float courtHight = 600.f;

	TArray<class AActor*> allAisInWorld;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* localPlayCamera;
	UPROPERTY(EditAnywhere)
	USceneComponent* playerASpawnLoc;
	UPROPERTY(EditAnywhere)
	USceneComponent* playerBSpawnLoc;
	UPROPERTY(EditAnywhere)
	USceneComponent* courtHightPos;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtOutOFBounds;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtOutOFBounds2;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtOutOFBounds3;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtOutOFBounds4;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtOutOfBoundsLid;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* matchEnterComp;

	class UQuadBallGameInstance* gameInstance;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* primaryRefereeMesh;

	UPROPERTY(EditAnywhere)
	class URefAnimInstance* mainRefAnimInstance;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* courtMesh;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* courtOBFloor;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* centerNetMesh;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* flagMesh2;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* aimerMeshPlayerA;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* aimerMeshPlayerB;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* playerACourtBoundary;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* playerBCourtBoundary;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* courtInBoundary;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* cleanSheet;

	UPROPERTY(EditAnywhere, Replicated)
	int ballStartSide = 0;
	UPROPERTY(EditAnywhere, Replicated)
	int playerBallResponsibility = 0; // 0 A, 1 B

	UFUNCTION()
		void OnRep_playerAScore();
	UFUNCTION()
		void OnRep_playerBScore();
	UFUNCTION()
		void OnRep_playerAGameScore();
	UFUNCTION()
		void OnRep_playerBGameScore();

	UPROPERTY(ReplicatedUsing = OnRep_playerAGameScore)
	int playerAGameScore = 0;
	UPROPERTY(ReplicatedUsing = OnRep_playerBGameScore)
	int playerBGameScore = 0;
	UPROPERTY(ReplicatedUsing=OnRep_playerAScore)
	int playerAScore = 0;
	UPROPERTY(ReplicatedUsing=OnRep_playerBScore)
	int playerBScore = 0;
	UPROPERTY(Replicated)
	int nextGameServer = 0;
	UPROPERTY(Replicated)
	bool needToSwitchSides = false;
	UPROPERTY(Replicated)
	bool needToEndGame = false;
	// 0 PVP, 1 Training
	UPROPERTY(EditAnywhere, Replicated)
		int gameMode = 1;
	UPROPERTY(Replicated)
		bool inSession = false;
	UPROPERTY(Replicated)
		int playersInQue = 0;

	UPROPERTY(EditAnywhere, Replicated)
	class AQuadBallCharacter* playerA;
	UPROPERTY(EditAnywhere, Replicated)
	class AQuadBallCharacter* playerB;

	UPROPERTY(EditAnywhere)
	class UAudioComponent* musicComponent;
	UPROPERTY(EditAnywhere)
	class UAudioComponent* effectComponent;
	UPROPERTY(EditAnywhere)
	class USoundCue* musicCue;
	UPROPERTY(EditAnywhere)
	class USoundCue* effectCue;
	UPROPERTY(EditAnywhere)
	class USoundCue* refVoiceCue;

	float refGreetCooldown = 5.f;
	float refGreetMax = 5.f;

	UFUNCTION(Client, Reliable)
	void Client_StartMusic();
	UFUNCTION(Client, Reliable)
	void Client_EndMusic();

	bool isLookingAt = false;



	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* ballBounceText;

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* playerResponsibilityText;

	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* playerAScoreText;

	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* playerBScoreText;
	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* playerAGameScoreText;
	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* playerBGameScoreText;
	UPROPERTY(EditAnywhere)
		class UTextRenderComponent* gameOverText;

	UPROPERTY(EditAnywhere)
	class AQuadBallBall* ball;

	UPROPERTY(EditAnywhere)
	class AQuadBallAutoCharacter* trainer;

	UPROPERTY(EditAnywhere)
	class USceneComponent* minimumSafeZ;
	UPROPERTY(EditAnywhere)
	class USceneComponent* trainerAimLocation;

	UPROPERTY(EditAnywhere)
	int debugStatus = 0;


	UPROPERTY(EditAnywhere)
	float minSafeNetDist = 20;

	bool firstInteract = false;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ballOBMarkSystem;
	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* ballOBMarkSystemComponent;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* pointIndicatingSys;
	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* pointIndicatingComp1;
	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* pointIndicatingComp2;
	UPROPERTY(EditAnywhere)
	class USceneComponent* indicateSpot1;
	UPROPERTY(EditAnywhere)
	class USceneComponent* indicateSpot2;

	AQuadBallCharacter* FetchOtherPlayer(AQuadBallCharacter* me);

	void PlayIndicateParticles();


	UPROPERTY(EditAnywhere)
	bool AICourtType = true;
	UPROPERTY(EditAnywhere)
	bool AIOnlyCourt = true;

	void CalculateCourtBasis();
	FVector courtForward = FVector::ForwardVector;
	FVector courtRight = FVector::RightVector;
	FVector courtUp = FVector::UpVector;
	FVector topRight = FVector::ZeroVector;
	FVector bottomLeft = FVector::ZeroVector;
	FBasisVectorMatrix courtBasisMatrix = FBasisVectorMatrix(courtForward, courtRight, courtUp, FVector::ZeroVector);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MULTICAST_StartMatch(const bool vsAi);
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_AwardPointEffect(const int playerTarget, const FString &reason, const bool award);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void LookAt(class AQuadBallCharacter* targChar, bool look);
	bool GetIsAICourt() { return AICourtType; };

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_CancelIndicateParticles();
	void CheckSwitchSide();
	bool CheckEndGame();

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetPlayerA(AQuadBallCharacter* player);
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetPlayerB(AQuadBallCharacter* player);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_QueForGame(class AQuadBallCharacter* player, bool challenge = false);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartMatch();

	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_UpdatePlayerScore();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ResetAllPlayerKicks();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetupPlayerForQuadBall(class AQuadBallCharacter* player, const int playerID);

	FORCEINLINE const FVector& GetTopRight() { return topRight; };
	FORCEINLINE const FVector& GetBottomLeft() { return bottomLeft; };
	FORCEINLINE const FBasisVectorMatrix& GetCourtBasis() { return courtBasisMatrix; };

	virtual int GetBallSide(class AQuadBallBall* ball);

	FORCEINLINE int GetPlayersInQue() { return playersInQue; };

	FORCEINLINE bool GetNeedToEndGame() { return needToEndGame; };
	FORCEINLINE class AQuadBallBall* GetActiveBall() { return ball; };
	FORCEINLINE int GetBallStartSide() { return ballStartSide; };

	FORCEINLINE bool GetIsInSession() { return inSession; };

	FORCEINLINE class UStaticMeshComponent* GetCourtFloor() { return courtMesh; };
	FORCEINLINE class UStaticMeshComponent* GetCourtOBFloor() { return courtOBFloor; };
	FORCEINLINE class UStaticMeshComponent* GetCourtNet() { return centerNetMesh; };
	FORCEINLINE class UBoxComponent* GetCleanSheet() { return cleanSheet; };
	FORCEINLINE class UBoxComponent* GetOB1() { return courtOutOFBounds; };
	FORCEINLINE class UBoxComponent* GetOB2() { return courtOutOFBounds2; };
	FORCEINLINE class UBoxComponent* GetOB3() { return courtOutOFBounds3; };
	FORCEINLINE class UBoxComponent* GetOB4() { return courtOutOFBounds4; };

	FORCEINLINE void SetSwitchSide(const bool b) { needToSwitchSides = b; };
	FORCEINLINE int GetPlayerAScore() { return playerAScore; };
	FORCEINLINE int GetPlayerBScore() { return playerBScore; };
	FORCEINLINE bool GetFirstInteract() { return firstInteract; };

	FORCEINLINE void SetPlayerA(class AQuadBallCharacter* a) { playerA = a; };
	FORCEINLINE void SetPlayerB(class AQuadBallCharacter* a) { playerB = a; };
	FORCEINLINE class AQuadBallCharacter* GetPlayerA() { return playerA; };
	FORCEINLINE class AQuadBallCharacter* GetPlayerB() { return playerB; };
	FORCEINLINE class AQuadBallAutoCharacter* GetTrainer() { return trainer; };

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_AwardPoint(const int playerTarget, const FString &reason);

	FORCEINLINE int GetGameMode() { return gameMode; };

	FORCEINLINE class UBoxComponent* GetCourtInBoundary() { return courtInBoundary; };

	FORCEINLINE int GetDebugStatus() { return debugStatus; };
	FORCEINLINE int IncrementDebugStatus() { return debugStatus = (debugStatus + 1) % 4; };

	FORCEINLINE class URefAnimInstance* GetMainRefAnimInstance() { return mainRefAnimInstance; };

	FORCEINLINE FVector GetPlayerASpawnLocation() { return playerASpawnLoc->GetComponentLocation(); };
	FORCEINLINE FVector GetPlayerBSpawnLocation() { return playerBSpawnLoc->GetComponentLocation(); };
	FORCEINLINE FVector GetCourtHeightPos() { return courtHightPos->GetComponentLocation(); };

	FORCEINLINE class UCameraComponent* GetLocalPlayCamera() { return localPlayCamera; };

	FVector GetPositionOnCourt(const int playerID, const float x, const float y, const int myC);
	bool GetBallSafeDistanceFromNet();



	FVector GetMinimumSafeZLocation();
	FVector GetTrainerAimLocation();

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_HidePoint();

	void ResetTrainerReturn();

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_UpdatePlayerResponsibilityDisplay(const int resp);
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_UpdateBallBounceDisplay(const int n, int m);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_SwitchSides();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndMatch();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_EndMatch();

	void MarkOBLocation(FVector &Loc);

	void SetPlayersForAllPlayers();
	void ShowVSAnimForAllPlayers();

	void ChangeMusicVolume(const float percent);
	void ChangeSFXVolume(const float percent);
	void ChangeMasterVolume(const float percent);
};

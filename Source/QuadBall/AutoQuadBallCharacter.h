// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuadBallCharacter.h"
#include "AutoQuadBallCharacter.generated.h"

/**
 * Automatically controlled Quad Ball Character
 */
UCLASS()
class QUADBALL_API AAutoQuadBallCharacter : public AQuadBallCharacter
{
	GENERATED_BODY()
	
public:

	AAutoQuadBallCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void Client_InitPlayerForQuadBall(class AQuadBallBall* ball, class AQuadBallCourt* court) override;
	virtual void Client_InitPlayerForIdle() override;
	virtual void Server_ResetAllKicks() override;
	virtual void InformPlayerOfBounce() override;
	void InitAIStatus(bool qBall);

protected:

	bool autoStartHandstand = false;
	int nextKickKey = 0;
	bool isQueryingPlay = false;

	UPROPERTY(EditAnywhere)
	float queryWaitTime = 500.f;
	UPROPERTY(EditAnywhere)
	float queryWaitTimeMax = 500.f;

	void QueryAutoPlay();

	bool bothAI;
	void HandleChallengeResponseTime(float DeltaTime);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	float kickCooldown = .5;
	float kickCooldownMax = .5;

	float specialMoveWaitTime = 6.f;

	bool servePending = false;
	UPROPERTY(Replicated)
	bool returned = false;
	int trainerStatus = -1;

	float chargeMod = 1.f;
	
	bool Towards();
	void AutoValidateKick();
	void HandleSpecialKick(float DeltaTime);
	void HandleMovement();
	void HandleKicking(float DeltaTime);
	void MoveToResetLocation();
	void WaitForPlayerServe();
	bool kickLeft = false;
	bool kickRight = false;
	bool handRight = false;
	bool handLeft = false;

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_InteractAnim_Auto(bool on, int interID = 0);

	UFUNCTION(Server, Reliable, WithValidation)
	void MoveCharacterToTarget(FVector target);

	UPROPERTY(EditAnywhere)
	float moveSpeedModifier = 1;

	UPROPERTY(EditAnywhere)
		float returnDist = 95;
	UPROPERTY(EditAnywhere)
		float minF = .25;
	UPROPERTY(EditAnywhere)
		float maxF = 1.2;

	UPROPERTY(EditAnywhere)
	bool autoPlayCharacter = false;

	void ReturnBall();

	UPROPERTY(EditAnywhere)
	FVector startLocation;
	FVector oldTarget;

public:

	FORCEINLINE bool GetIsQuerying() { return isQueryingPlay; };
	FORCEINLINE void SetReturned(bool iN) { returned = iN; };

};

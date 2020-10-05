// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadBallBall.generated.h"

UCLASS()
class QUADBALL_API AQuadBallBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuadBallBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	const FName TYPE_PARAM = FName("Type");
	const FName MAST_VOL = FName("MAST_VOL");
	const FName SFX_VOL = FName("SFX_VOL");
	const float STANDARD_HIT = 0;
	const float NET_HIT = 1.1;
	const int TWO_BOUNCE_STATUS = 1;
	const int ONE_BOUNCE_STATUS = 2;
	const int NOT_PLAY_STATUS = 0;

	UPROPERTY(Replicated)
	bool updateLoc = true;


	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ballMesh;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* ballIndicatorMesh;
	UPROPERTY(EditAnywhere)
	float maxScaleSize = 20;
	UPROPERTY(EditAnywhere)
	float maxHight = 1000;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* ballHitComponent;
	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* specialDrivenComponent;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* specialDrivenSystem;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ballResetSystem;
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ballUncleanSystem;
	FVector ballLastFramePos;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ballRespawnerLocationA;
	UPROPERTY(EditAnywhere)
	class USceneComponent* ballRespawnerLocationB;

	float doubleBounceMax = .06;
	float doubleBounceRemain = 0;

	UPROPERTY(EditAnywhere)
	class AQuadBallCourt* court;

	UPROPERTY(EditAnywhere)
	class UAudioComponent* ballHitAudioComponent;
	UPROPERTY(EditAnywhere)
	class USoundCue* ballHitSoundCue;

	bool hasValidCourt = false;

	// 0 A, 1 B, -1 Not set


	UFUNCTION()
	virtual void OnBallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBeginClean(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// -1 Pre play, 1 in play state 1 (can bounce twice), 2 in play state 2 (can bounce once), 0 play over
	UPROPERTY(Replicated)
	bool readyToServe = false;
	UPROPERTY(Replicated)
	bool isServe = false;
	UPROPERTY(Replicated)
	bool hasKickedServe = false;
	UPROPERTY(Replicated)
	bool newServe = true;
	UPROPERTY(Replicated)
	int ballPlayStatus = -1;
	UPROPERTY(Replicated)
	bool juggle = false;
	UPROPERTY(Replicated)
	bool isSpecialDriven = false;
	UPROPERTY(Replicated)
	int currentBallSide = -1;
	UPROPERTY(Replicated)
	bool clean = true;
	UPROPERTY(Replicated)
	int ballBounceCount = -1;
	UPROPERTY(Replicated)
	int playerBallResponsibility = 0;
	UPROPERTY(Replicated)
	bool needsToBeReset = false;

	UFUNCTION(NetMulticast, Reliable)
	void BallSetPhysics(bool on);

	// Only play bounces count



	UPROPERTY(Replicated)
	bool oBLocMarked = false;
	FVector oBLoc;
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_MarkOBLoc(FVector loc);
	void ClearOBLoc();



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// CALLED FROM CLIENT
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestBallReset();
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestServe();

	// MULTICAST OUT TO ALL
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_UpdateClientBallLoc(const FVector ballLo, const FRotator rotc);
	UFUNCTION(NetMulticast, WithValidation, Reliable)
	virtual void MULTICAST_ResetBallLocation(int id);
	UFUNCTION(NetMulticast, WithValidation, Reliable)
	virtual void MULTICAST_ServeBall();
	UFUNCTION(Server, WithValidation, Reliable)
	virtual void Server_DoBallAction(const FVector& inpulse);
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlayBallHitEffect(const float inVal);
	UFUNCTION(NetMulticast, Unreliable)
	void MULTICAST_PlaySpecialBallHit(class UParticleSystem* inSys);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_BallHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FVector GetBallLocation();

	UFUNCTION(NetMulticast, WithValidation, Reliable)
	virtual void DoBallReset();
	virtual void UpdateBallBounceCount(int c);
	virtual int GetNewBallSide();
	virtual void SetBallInteractivity(bool set);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OverlapClean(UPrimitiveComponent* OtherComp);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_SetClean(bool inVal);
	virtual bool ImpactInBounds(FVector impactZone);

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_BallInteractivity(bool set);


	void DebugResetBall();
	void KickAtAngleWithForce(float rad, FVector dir, float mag);

	FORCEINLINE void SetNeedsToBeReset(bool in) { needsToBeReset = in; };
	FORCEINLINE bool GetNeedsToBeReset() { return needsToBeReset; };
	FORCEINLINE int GetCurrentBallSide() { return currentBallSide; };
	FORCEINLINE void SetBallSide(int newBallSide) { currentBallSide = newBallSide; };
	FORCEINLINE class UStaticMeshComponent* GetBallMesh() { return ballMesh; };
	
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetIsSpecialDriven(bool in);
	FORCEINLINE bool GetIsSpecialDriven() { return isSpecialDriven; };
	FORCEINLINE void SetReadyToServe(bool in) { readyToServe = in; };
	FORCEINLINE bool GetReadyToServe() { return readyToServe; };
	FORCEINLINE bool GetHasKickedServe() { return hasKickedServe; };
	FORCEINLINE int GetBallPlayStatus() { return ballPlayStatus; };
	FORCEINLINE virtual void SetBallPlayStatus(int inVal) { ballPlayStatus = inVal; };
	FORCEINLINE int GetBallBounceCount() { return ballBounceCount; };
	FORCEINLINE void SetBallBounceCount(int inVal) { UpdateBallBounceCount(inVal); };

	FORCEINLINE int GetPlayerBallResponsibility() { return playerBallResponsibility; };
	FORCEINLINE virtual void InitPlayerBallResponsibility(int inVal) { playerBallResponsibility = inVal; };
	virtual void SetPlayerBallResponsibility(int inVal, int bounce, bool force = false);

	FORCEINLINE bool GetJuggleStatus() { return juggle; };
	FORCEINLINE void SetJuggleStatus(bool inVal) { juggle = inVal; };

	FORCEINLINE bool GetClean() { return clean; };


	FORCEINLINE bool GetNewServe() { return newServe; };
	FORCEINLINE void SetNewServe(bool inVal) { newServe = inVal; };

	FORCEINLINE bool GetIsServe() { return isServe; };

	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetPositionIndicatorVisibleStatus(bool vis);

	void ChangeSFXVolume(float percent);
	void ChangeMasterVolume(float percent);
	
};

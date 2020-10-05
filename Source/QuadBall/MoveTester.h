// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MoveTester.generated.h"

UCLASS()
class QUADBALL_API AMoveTester : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMoveTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* rootC;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* restartPoint;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCameraComponent* playerCamera;
	UPROPERTY(EditAnywhere, Category = "Components")
	class USpringArmComponent* playerCameraBoom;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* playerAimerComp;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UStaticMeshComponent* ball;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCurveFloat* zCurve;
	UPROPERTY(EditAnywhere, Category = "Components")
	class UCurveFloat* xCurve;

	UPROPERTY(EditAnywhere, Category = "Values")
	float maxZoom = 3000;
	UPROPERTY(EditAnywhere, Category = "Values")
	float minZoom = 300;
	UPROPERTY(EditAnywhere, Category = "Values")
	float zoomAmount = 5;
	UPROPERTY(EditAnywhere, Category = "Values")
	float standardVel = 1000;

	bool controlMode = 0;
	bool simPhysics = 0;

	void Zoom(float inVal);
	void MoveR(float inVal);
	void MoveU(float inVal);
	void Reset();
	void SetSimPhysics();
	void ChangeControlMode();
	void Send();

	float timeInAir = 0;
	float oldTimeInAir = 0;
	float totalDist = 0;
	float distTraveled = 0;
	float oldDistTraveled = 0;
	float multFactor = 1;
	bool isJumping = false;

	FVector vertMove = FVector::ZeroVector;
	FVector horizMove = FVector::ZeroVector;
	FVector aimDir = FVector::ZeroVector;
	FVector rightDir = FVector::ZeroVector;
	FVector totalMove = FVector::ZeroVector;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "QuadBallMenuPawn.generated.h"

UCLASS()
class QUADBALL_API AQuadBallMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AQuadBallMenuPawn();

protected:

	const int MAIN_MODE = 1;
	const int OPTION_MODE = 2;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class USoundClass* MSTR_SoundClass;
	class USoundClass* SFX_SoundClass;
	class USoundClass* MUS_SoundClass;

	UPROPERTY(EditAnywhere)
	class USceneComponent* rootC;

	UPROPERTY(EditAnywhere)
	class USceneComponent* p1Transform;
	UPROPERTY(EditAnywhere)
	class USceneComponent* p1TransformCharaChange;
	UPROPERTY(EditAnywhere)
	class USceneComponent* pointToDirectionTransform;
	UPROPERTY(EditAnywhere)
	class USceneComponent* p2Transform;
	UPROPERTY(EditAnywhere)
	class USceneComponent* p2TransformCharaChange;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* camera;
	UPROPERTY(EditAnywhere)
	class USceneComponent* cameraTransformStandard;
	UPROPERTY(EditAnywhere)
	class USceneComponent* cameraTransformOptions;

	UPROPERTY(EditAnywhere)
	class UAudioComponent* menuAmbientComponent;
	UPROPERTY(EditAnywhere)
	class UAudioComponent* menuMusicComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* menuAmbientCue;
	UPROPERTY(EditAnywhere)
	class USoundCue* menuMusicCue;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* p1SplashPlane;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* p2SplashPlane;



	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> UIWidgetBase;
	class UQuadBallMenuUI* UIWidget;

	class UQuadBallGameInstance* gameInstance;

	UPROPERTY(EditAnywhere)
	class AQuadBallCharacter* p1;

	UPROPERTY(EditAnywhere)
	class AQuadBallCharacter* p2;

	void SetupPlayerState(bool idle);

	void MenuStartGame();

	void InterpCamera(float DeltaTime);

	float DistComponents(USceneComponent* comp1, USceneComponent* comp2);

	bool modeTransitionActive = false;
	int menuMode = 1;
	bool gameStarted = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	class AQuadBallCharacter* InstantPlayer1(UClass* inC);
	UFUNCTION(BlueprintCallable)
	class AQuadBallCharacter* InstantPlayer2(UClass* inC);

	UFUNCTION(BlueprintCallable)
	void ChangeMenuMode();
	UFUNCTION(BlueprintCallable)
	void MoveCharacters();

	UFUNCTION(BlueprintCallable)
	void ShowMenuUI();

	UFUNCTION(BlueprintCallable)
	void SetVolumeLevels(float SFX, float MUSIC, float MASTER);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAudioComponent* GetMusicSound() { return menuMusicComponent; };

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAudioComponent* GetAmbientSound() { return menuAmbientComponent; };
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h" 
#include "Engine/Texture2D.h"
#include "QuadBallCharacterAnimInstance.h"
#include "QuadBallCharacter.h"
#include "QuadBallGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UQuadBallGameInstance();



	class USoundClass* MSTR_SoundClass;
	class USoundClass* SFX_SoundClass;
	class USoundClass* MUS_SoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* playerAMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClass* playerAAnimInst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* playerACharacterPortait;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerAName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMesh* playerBMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClass* playerBAnimInst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* playerBCharacterPortait;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString playerBName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AQuadBallCharacter* playerACharData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AQuadBallCharacter* playerBCharData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClass* playerAClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UClass* playerBClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int gameMode = -1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int debugMode = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float defaultSFXVol = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float defaultMasterVol = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float defaultMusicVol = .69;

	void SetSFXVol(float percent);
	void SetMUSVol(float percent);
	void SetMasterVol(float percent);
};

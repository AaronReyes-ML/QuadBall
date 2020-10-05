// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "QuadBallCharacter.h"
#include "Engine/Texture2D.h"
#include "QuadBallCharacter.h"
#include "QuadBallUI.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UQuadBallUI : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AQuadBallCharacter* thisOwner;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetAimerLocation(int playerID, float x, float y, int myC);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetSpinAimerLocation(float x, float y, int myC);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowUI_QuadBall();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HideUI_QuadBall();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowPause();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HidePause();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowPoint(const FString &Pname, const FString &reason);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HidePoint();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateChargeBar(float min, float max, float current);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowPlayQuadBallUI(int type);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HidePlayQuadBallUI();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetOwner(class AQuadBallCharacter* own);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetVsAI(bool ai);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetPlayers(class ACharacter* player1, class ACharacter* player2, class UTexture2D* p1_Splash, class UTexture2D* p2_Splash);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowVSAnim();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdatePlayerResponsibility(class ACharacter* playerToShow, class ACharacter* calledFrom);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void RefreshAllCards();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClearCard(int id);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AddCardToHistory(int id, bool jug, bool bad);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClearHistory();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateATB(float ATB, float maxATB);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMoveType(int moveKey);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowPlayerResponsibiliy(bool in);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetVolumeSliders(float SFX, float MUSIC, float MASTER);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateCurrentTutorial(int index);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ToogleQ(bool on);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CanChallenge(bool on);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChallengeSent(bool on, class AQuadBallCharacter* otherChar);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChallengeRec(bool on, class AQuadBallCharacter* otherChar);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ChallengeAccepted();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CourtNotFoundError();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NoOtherPlayerError();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TutNext();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TutPrev();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TutVis();

};

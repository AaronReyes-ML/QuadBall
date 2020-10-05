// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RefAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API URefAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PointA();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PointB();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ClearPoint();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetLooking(bool look, class USceneComponent* lookAtOb);
	
};

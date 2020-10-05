// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "InteractAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADBALL_API UInteractAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void DoSit();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Cycle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetType(int t);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int type = 101;

	
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractAnimInstance.h"
#include "WorldInteractable.generated.h"

UCLASS()
class QUADBALL_API AWorldInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldInteractable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const;

	UPROPERTY(EditAnywhere)
	class UClass* interactAnimInstance;

	UPROPERTY(EditAnywhere)
	int interactType = 101;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* interactVolume;

	UPROPERTY(EditAnywhere)
	class USceneComponent* interactPoint;

	UPROPERTY(EditAnywhere)
	class USceneComponent* exitPoint;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* interactableMesh;

	UPROPERTY(Replicated)
	bool used = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE
	class USceneComponent* GetInteractPoint() { return interactPoint; };
	FORCEINLINE
	class USceneComponent* GetExitPoint() { return exitPoint; };

	FORCEINLINE
	int GetInteractType() { return interactType; };

	FORCEINLINE bool GetUsed() { return used; };

	FORCEINLINE
	class UClass* GetAnimInstance() { return interactAnimInstance; };

	void SetCollisionInteract(bool in);

	UFUNCTION(Server, Reliable, WithValidation)
	void SetUsed(bool u);

	
};

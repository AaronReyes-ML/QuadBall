// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldInteractable.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Components/BoxComponent.h"
#include "Engine.h"
#include "UnrealNetwork.h"


// Sets default values
AWorldInteractable::AWorldInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	interactableMesh = CreateDefaultSubobject<UStaticMeshComponent>("Interactable mesh");
	interactableMesh->SetRelativeLocation(FVector::ZeroVector);
	SetRootComponent(interactableMesh);

	interactPoint = CreateDefaultSubobject<USceneComponent>("interact location");
	interactPoint->SetupAttachment(interactableMesh);
	interactPoint->SetRelativeLocation(FVector::ZeroVector);

	exitPoint = CreateDefaultSubobject<USceneComponent>("Exit location");
	exitPoint->SetupAttachment(interactableMesh);
	exitPoint->SetRelativeLocation(FVector::ZeroVector);

	interactVolume = CreateDefaultSubobject<UBoxComponent>("Interact volume");
	interactVolume->SetCollisionObjectType(ECC_GameTraceChannel7);
	interactVolume->SetupAttachment(RootComponent);

	SetReplicates(true);
}

void AWorldInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldInteractable, used);

}

bool AWorldInteractable::SetUsed_Validate(bool u) { return true; }

void AWorldInteractable::SetUsed_Implementation(bool u)
{
	used = u;
}

// Called when the game starts or when spawned
void AWorldInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldInteractable::SetCollisionInteract(bool in)
{
	if (in)
	{
		interactableMesh->SetCollisionObjectType(ECC_GameTraceChannel9);
		interactableMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		interactableMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	}
	else
	{
		interactableMesh->SetCollisionObjectType(ECC_WorldStatic);
	}

}


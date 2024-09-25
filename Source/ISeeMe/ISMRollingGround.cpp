// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRollingGround.h"
#include "GameFramework/RotatingMovementComponent.h"


// Sets default values
AISMRollingGround::AISMRollingGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Movement"));

	RootComponent = GroundMesh;

	Movement->RotationRate = FRotator(0.f, RotateSpeed, 0.f);
}

void AISMRollingGround::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		SetActorTickEnabled(false);
}

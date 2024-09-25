// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMMovingGround.h"

AISMMovingGround::AISMMovingGround()
{
	PrimaryActorTick.bCanEverTick = true;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));

	RootComponent = GroundMesh;
}

void AISMMovingGround::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		SetActorTickEnabled(false);

	RemainTime = ChangeTime;
}

void AISMMovingGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalOffset(Velocity * DeltaTime);

	RemainTime -= DeltaTime;
	if (RemainTime <= 0.f)
	{
		ResetTimer();
	}
}

void AISMMovingGround::ResetTimer()
{
	RemainTime = ChangeTime;

	Velocity *= -1.f;
}
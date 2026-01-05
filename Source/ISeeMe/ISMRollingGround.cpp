// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRollingGround.h"
#include "GameFramework/RotatingMovementComponent.h"


// Sets default values
AISMRollingGround::AISMRollingGround()
{
	// 회전을 위한 MovementComponent의 Tick은 별도로 발생, 액터의 Tick만 비활성화
	PrimaryActorTick.bCanEverTick = false;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));
	Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Movement"));

	RootComponent = GroundMesh;
}

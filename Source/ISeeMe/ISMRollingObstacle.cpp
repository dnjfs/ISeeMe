// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRollingObstacle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AISMRollingObstacle::AISMRollingObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	ControlPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Control Point"));
	ControlPoint->SetIsReplicated(true);
	RootComponent = ControlPoint;

	RotatingObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotating Object"));
	RotatingObject->SetupAttachment(ControlPoint);
	RotatingObject->SetIsReplicated(true);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AISMRollingObstacle::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		InitialRotation = RotatingObject->GetComponentRotation();

		RotatingMovement->RotationRate = RotationSpeed;

		FVector ObjectOffset = FVector::Zero();

		if (RotationSpeed.Pitch != 0) // Y축 기준 회전
		{
			ObjectOffset.Z += RotatorArmLength;
		}
		if (RotationSpeed.Yaw != 0) // Z축 기준 회전
		{
			ObjectOffset.X += RotatorArmLength;
		}
		if (RotationSpeed.Roll != 0) // X축 기준 회전
		{
			ObjectOffset.Y += RotatorArmLength;
		}

		RotatingObject->SetRelativeLocation(ObjectOffset);
	}
	if (!HasAuthority())
		SetActorTickEnabled(false);
}


// Called every frame
void AISMRollingObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
		RotatingObject->SetWorldRotation(InitialRotation);
}
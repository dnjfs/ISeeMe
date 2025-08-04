// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMRollingObstacle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ISeeMeCharacter.h"

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
		SetRelativeObjectLocation();

		RotatingObject->OnComponentBeginOverlap.AddDynamic(this, &AISMRollingObstacle::PushCharacter);
	}
	if (!HasAuthority())
		SetActorTickEnabled(false);
}

// Called every frame
void AISMRollingObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
#if WITH_EDITOR
		EditorTick(DeltaTime);
#endif
		RotatingObject->SetWorldRotation(InitialRotation);
		ObjectOldLoc = ObjectNewLoc;
		ObjectNewLoc = RotatingObject->GetComponentLocation();
	}
}

void AISMRollingObstacle::SetRelativeObjectLocation()
{
	FVector ObjectOffset = FVector::Zero();

	if (RotationSpeed.Pitch != 0) // Y축 기준 회전
		ObjectOffset.Z += 1.f;

	if (RotationSpeed.Yaw != 0) // Z축 기준 회전
		ObjectOffset.X += 1.f;

	if (RotationSpeed.Roll != 0) // X축 기준 회전
		ObjectOffset.Y += 1.f;

	//정규화
	ObjectOffset.Normalize();
	ObjectOffset *= RotatorArmLength;

	RotatingObject->SetRelativeLocation(ObjectOffset);
}

void AISMRollingObstacle::PushCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OverlappedComponent)
		return;

	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		FVector StartLocation = OverlappedComponent->GetComponentLocation();
		FVector EndLocation = Character->GetActorLocation();
		FVector PushingDirection = (EndLocation - StartLocation).GetSafeNormal();

		Character->LaunchCharacter(PushingDirection * PushingPower, true, true);
	}
}

void AISMRollingObstacle::EditorTick(float DeltaTime)
{
	SetRelativeObjectLocation();
}
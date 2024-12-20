// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMRollingObstacle.generated.h"

UCLASS()
class ISEEME_API AISMRollingObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMRollingObstacle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Category = RotationSettings)
	FRotator RotationSpeed;

	UPROPERTY(EditAnywhere, Category = RotationSettings)
	float RotatorArmLength;

	UPROPERTY(EditDefaultsOnly)
	USceneComponent* ControlPoint;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* RotatingObject;

	UPROPERTY(VisibleAnywhere)
	class URotatingMovementComponent* RotatingMovement;

	FRotator InitialRotation;
};

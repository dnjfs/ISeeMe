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

	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* BumpSound;

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

	FVector ObjectNewLoc;
	FVector ObjectOldLoc;

	UPROPERTY(EditAnywhere)
	float PushingPower;

	void EditorTick(float DeltaTime);

	void SetRelativeObjectLocation();

	UFUNCTION()
	void PushCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMRollingGround.generated.h"

class URotatingMovementComponent;

UCLASS()
class ISEEME_API AISMRollingGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMRollingGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URotatingMovementComponent> Movement;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float RotateSpeed = 100.f;
};

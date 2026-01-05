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

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<URotatingMovementComponent> Movement;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMMovingGround.generated.h"

UCLASS()
class ISEEME_API AISMMovingGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMMovingGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ResetTimer();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	FVector Velocity;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float ChangeTime = 5.f;

	float RemainTime = 0.f;
};

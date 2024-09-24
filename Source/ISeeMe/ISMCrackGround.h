// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMCrackGround.generated.h"

UCLASS()
class ISEEME_API AISMCrackGround : public AActor
{
	GENERATED_BODY()
	
public:	
	AISMCrackGround();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnCracked();

	void ResetTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float CrackTime = 3.f;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float DormantTime = 10.f;

	float RemainTime = 0.f;
};

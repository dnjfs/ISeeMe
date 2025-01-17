// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ISMCrackGround.generated.h"

class UGeometryCache;
class UGeometryCacheComponent;
class UAudioComponent;

UCLASS()
class ISEEME_API AISMCrackGround : public AActor
{
	GENERATED_BODY()
	
public:	
	AISMCrackGround();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnCracked();

	void ResetTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeCrack(UGeometryCache* ChangeMesh);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetCracking(bool bInCracking);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnCrackPart();

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCacheComponent> GeometryCacheComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCache> FirstCrack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCache> HalfCrack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCache> MostCrack;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCache> FallingGeometry;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float CrackTime = 3.f;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float DormantTime = 10.f;

	float RemainTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundWave> CrackingSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundWave> BrokenSound;
};

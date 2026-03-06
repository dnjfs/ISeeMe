// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abstract/ResetableActor.h"
#include "Components/BoxComponent.h"
#include "ISMCrackGround.generated.h"

class UGeometryCache;
class UGeometryCacheComponent;
class UAudioComponent;

UENUM(BlueprintType)
enum class ECrackState : uint8
{
	Idle,
	Start,
	Half,
	Most,
	Cracked,
};

UCLASS()
class ISEEME_API AISMCrackGround : public AResetableActor
{
	GENERATED_BODY()
	
public:	
	AISMCrackGround();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnReset() override;

	UFUNCTION()
	void OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnCracked();

	void ResetTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeCrack(UGeometryCache* ChangeMesh);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetCracking(ECrackState InCrackState);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnCrackPart();

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

	ECrackState CrackState = ECrackState::Idle;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundWave> CrackingSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundWave> BrokenSound;

	FTimerHandle CrackTimerHandle;
};

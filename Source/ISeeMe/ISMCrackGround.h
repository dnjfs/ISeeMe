// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/SphereComponent.h"
#include "Field/FieldSystemComponent.h"
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

	void CrackDestroyTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastCrackAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeCrack(UMaterialInterface* ChangeMaterial);

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
	TObjectPtr<UGeometryCollectionComponent> CrackGeometry;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> FirstCrackMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> HalfCrackMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> MostCrackMaterial;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFieldSystemActor> CrackPartClass;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float CrackTime = 3.f;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float DormantTime = 10.f;

	AFieldSystemActor* CrackPartActor;

	float RemainTime = 0.f;

	float CrackLifeTime = 3.f;

	bool bIsCrack = false;

	int8 crackStep = 1;
};

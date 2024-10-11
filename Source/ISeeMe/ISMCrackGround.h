// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/SphereComponent.h"
#include "ISMCrackGround.generated.h"

UCLASS()
class ISEEME_API AISMCrackGround : public AActor
{
	GENERATED_BODY()
	
public:	
	AISMCrackGround();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isCrack = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void OnCracked();

	void ResetTimer();

	void CrackTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastCrackAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetClack(float crackState);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetClacking(bool bInCracking);

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
	TObjectPtr<UMaterialInterface> CrackingMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> FirstCrackMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> HalfCrackMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> MostCrackMaterial;


	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float CrackTime = 3.f;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float DormantTime = 10.f;

	float RemainTime = 0.f;
};

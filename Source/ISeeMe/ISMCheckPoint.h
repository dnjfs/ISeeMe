// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "ISMCheckPoint.generated.h"

UCLASS()
class ISEEME_API AISMCheckPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AISMCheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeMaterial(UMaterialInterface* ChangeMaterial);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void ServerCallSwapCamera();

	UFUNCTION()
	void InitCheckPoint();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CheckPointMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spawn1PPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spawn2PPlayer;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> CheckMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> AllCheckMaterial;

private:
	int DetectPlayer = 0;
};

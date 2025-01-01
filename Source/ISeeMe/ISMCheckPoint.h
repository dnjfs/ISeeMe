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

	/* Change Material to the Server, Client*/
	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeMaterial(int CurDetect);

public:

	/*Init Check Point from the client to the server*/
	UFUNCTION(Server, Reliable)
	void ServerInitCheckPoint();

	/*Init Check Point to the server*/
	UFUNCTION()
	void InitCheckPoint();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	/**Check point Range**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerVolume;

	/**Check point Mesh**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CheckPointMesh;

	/** When input check point, Go back spawn point with 1p player**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spawn1PPlayer;

	/** When input check point, Go back spawn point with 2p player**/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Spawn2PPlayer;

	/*When no one detect, change material*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> BaseMaterial;

	/*When one detect, change material*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> CheckMaterial;

	/*When all detect, change material*/
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> AllCheckMaterial;

private:
	int DetectPlayer = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "ISMSwitch.generated.h"

UCLASS()
class ISEEME_API AISMSwitch : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AISMSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastMove(float DeltaTime);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeMaterial(bool bCheck);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* Switch Platform */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SwitchMesh;

	/* Switch Trigger */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* TriggerVolume;

	/* Move Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MoveMesh;

	/* When don't detect, move mesh is base material */
	UPROPERTY(EditAnywhere, Category = "Move")
	TObjectPtr<UMaterialInterface> BaseMaterial;

	/* When detect, move mesh is check material */
	UPROPERTY(EditAnywhere, Category = "Move")
	TObjectPtr<UMaterialInterface> CheckMaterial;

	/* MoveMesh Move Direction */
	UPROPERTY(EditAnywhere, Category = "Move")
	FVector DirectionVector = FVector(0, 0, 0);

	/* MoveMesh Move Time */
	UPROPERTY(EditAnywhere, Category = "Move")
	float MoveTime = 3.0f;

private:
	/* Current Player in TriggerVolume*/
	int CurDetectPlayer = 0;

	/* When don't detect, MoveMeshLocation*/
	FVector StartLocation = FVector(0, 0, 0);

	/* When detect, MoveMeshLocation*/
	FVector EndLocation = FVector(0, 0, 0);

	/* Target Location*/
	FVector TargetLocation = FVector(0, 0, 0);

	/* MoveMesh Speed*/
	float MoveSpeed = 1.0f;
};

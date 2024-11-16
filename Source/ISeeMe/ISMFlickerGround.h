// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMFlickerGround.generated.h"

UCLASS()
class ISEEME_API AISMFlickerGround : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMFlickerGround();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void ResetTimer();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAwake(bool bInAwake);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSetTransparency(float InOpacity);

protected:

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> BaseMaterial;
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> GroundMesh;

	bool bIsDormancy = false;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float FlickerInterval = 5.f;

	UPROPERTY(EditInstanceOnly, Category = GroundOption)
	float DormantTime = 3.f;

	float RemainTime = 0.f;

	FTimerHandle TimerHandle;
};

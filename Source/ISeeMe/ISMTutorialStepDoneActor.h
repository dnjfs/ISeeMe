// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "ISMTutorialStepDoneActor.generated.h"

UCLASS()
class ISEEME_API AISMTutorialStepDoneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMTutorialStepDoneActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**Check point Range**/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere)
	int32 MaxDetectPlayer = 2;

	UFUNCTION()
	void InitStepDone();

	UPROPERTY(EditAnywhere, Category = "Step")
	int32 TutorialStep = 2;
	
	UFUNCTION()
	void ChangeCollisionState();

private:
	int DetectPlayer = 0;

	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};

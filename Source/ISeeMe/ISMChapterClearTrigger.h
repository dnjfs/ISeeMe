// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMChapterClearTrigger.generated.h"

UCLASS()
class ISEEME_API AISMChapterClearTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMChapterClearTrigger();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CompleteChapter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSaveChapterNo();

	UFUNCTION()
	void MoveToNextChapter();

private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* TriggerBox;

	int DetectedPlayerCount;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMChapterClearTrigger.generated.h"

DECLARE_DELEGATE_OneParam(FOnClearUpdated, bool);

UCLASS()
class ISEEME_API AISMChapterClearTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMChapterClearTrigger();

	FOnClearUpdated OnClearUpdated;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ClearSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void CompleteChapter();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSaveChapterNo();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlaySound();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastShowLoadingWidget();

	void MoveToNextChapter();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Move")
	FString NextChapter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Move")
	int NextChapterNo;

private:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* TriggerBox;

	int DetectedPlayerCount;
};

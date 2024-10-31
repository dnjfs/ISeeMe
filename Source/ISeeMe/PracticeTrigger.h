// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSequence.h"  
#include "Components/BoxComponent.h"
#include "LevelSequencePlayer.h"
#include "PracticeTrigger.generated.h"

UCLASS()
class ISEEME_API APracticeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APracticeTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UCinematicUI> UIWidgetClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere)
	ULevelSequence* LevelSequence;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void ActivateInput();

	UFUNCTION()
	void ChargeSkip();

	UFUNCTION()
	void Skip();


private:
	UPROPERTY()
	TObjectPtr<class UCinematicUI> UIWidgetInstance;

	ULevelSequencePlayer* LevelSequencePlayer;
};

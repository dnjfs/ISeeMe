// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ISMSwapViewItem.h"
#include "ISMGameState.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	AISMGameState();

	virtual void BeginPlay() override;

public:
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	AISMSwapViewItem* SwapViewItem; // Current Own

	UPROPERTY(Replicated)
	TArray<AISMSwapViewItem*> UsedSwapViewItems; // Before return items, used item

	UPROPERTY(Replicated)
	AISMSwapViewItem* SaveSwapViewItem; // When check point, save item

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bAcqCheckPoint;

	UPROPERTY(Replicated)
	TArray<TSubclassOf<APawn>> SelectedPawnClasses;

	UFUNCTION()
	void AddSelectedPawnClass(const TSubclassOf<APawn> &NewPawn);

};

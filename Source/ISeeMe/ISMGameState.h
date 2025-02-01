// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ISMSwapViewItem.h"
#include "ISMGameState.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnSwapItemUpdated, bool);

UCLASS()
class ISEEME_API AISMGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	AISMGameState();

public:
	FOnSwapItemUpdated OnSwapItemUpdated; 

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	AISMSwapViewItem* SwapViewItem; // Current Own

	UPROPERTY(Replicated)
	TArray<AISMSwapViewItem*> UsedSwapViewItems; // Before return items, used item

	UPROPERTY(Replicated)
	AISMSwapViewItem* SaveSwapViewItem; // When check point, save item

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetSwapViewItem(AISMSwapViewItem* AcqSwapViewItem);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastReturnSwapViewItem();

	bool bAcqCheckPoint; // Check Check Point
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ISMSwapViewItem.h"
#include "ISMGameState.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FOnSwapItemUpdated, bool, bool);

UCLASS()
class ISEEME_API AISMGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	AISMGameState();

public:
	FOnSwapItemUpdated OnSwapItemUpdated;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnCharacterReturned();
	void OnCheckPointSaved();
	void OnItemAcquired(AISMSwapViewItem* AcqSwapViewItem);
	void OnItemUsed();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayItemSound(bool bIsPlay);

	void UpdateItemUIState();

	UFUNCTION()
	void OnRep_SwapViewItem();

	bool HasSwapViewItem() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<USoundBase> ItemTimerSound;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> TimerAudioComponent;

	UPROPERTY(ReplicatedUsing = OnRep_SwapViewItem)
	AISMSwapViewItem* SwapViewItem; // Current Own

	UPROPERTY(ReplicatedUsing = OnRep_SwapViewItem)
	TArray<AISMSwapViewItem*> UsedSwapViewItems; // Before return items, used item

	UPROPERTY(ReplicatedUsing = OnRep_SwapViewItem)
	AISMSwapViewItem* SaveSwapViewItem; // When check point, save item
};

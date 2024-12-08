// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
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

public:
	UPROPERTY(Replicated)
	int32 HasSwapItem;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

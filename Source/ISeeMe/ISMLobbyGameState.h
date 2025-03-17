// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ISMLobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	int32 HojinSelect = 0; // Num of Hojin Select

	UPROPERTY(BlueprintReadWrite)
	int32 MimiSelect = 0; // Num of Mimi Select

	TArray<bool> bSelectedHojin = { false,false };

	TArray<bool> bSelectedMimi = { false,false };

	UFUNCTION()
	void InitSelect();
};

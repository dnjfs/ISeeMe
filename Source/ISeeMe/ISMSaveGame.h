// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ISMSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UISMSaveGame();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	int UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
	FVector PlayerPos; // For Test
};

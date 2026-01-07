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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CurrChapterNo;

	// 클리어한 최대 챕터
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int MaxChapterNo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FName CheckPointID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsFirstLaunch;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bEnableSmoothCharacterMovement;
};

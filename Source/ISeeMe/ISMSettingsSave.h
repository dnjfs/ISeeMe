// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ISMSettingsSave.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMSettingsSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float BGMValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float SFXValue;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UISMSettingsSave();
};

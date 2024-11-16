// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void ToggleWidget(APlayerController* PlayerController);

protected:
	UFUNCTION(BlueprintCallable)
	void OnBackButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnGuideButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnOptionButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnControlButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};

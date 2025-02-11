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

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USlider* BGMSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USlider* SFXSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* BGMValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SFXValue;

protected:
	UFUNCTION(BlueprintCallable)
	void InitSoundSetting(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass);

	UFUNCTION(BlueprintCallable)
	void ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundClass, float Volume, FString Mode);

	UFUNCTION(BlueprintCallable)
	void OnBackButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnGuideButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnOptionButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnGraphicOptionButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnSoundOptionButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnControlButtonPressed();

	UFUNCTION(BlueprintCallable)
	void OnExitButtonPressed();
};

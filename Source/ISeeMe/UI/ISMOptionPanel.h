// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMOptionPanel.generated.h"

class USaveGame;
class USoundMix;

UCLASS()
class ISEEME_API UISMOptionPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ResetPageSwitcher();

	UFUNCTION(BlueprintCallable)
	void InitSoundSetting(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass);

	UFUNCTION(BlueprintCallable)
	void ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundClass, float Volume, FString Mode);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USlider* BGMSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USlider* SFXSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* BGMValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SFXValue;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* OptionSwitcher;
};

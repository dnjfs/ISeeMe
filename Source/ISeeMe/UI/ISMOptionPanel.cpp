// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMOptionPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include <ISeeMe/ISMSettingsSave.h>


void UISMOptionPanel::ResetPageSwitcher()
{
	OptionSwitcher->SetActiveWidgetIndex(0);
}

void UISMOptionPanel::InitSoundSetting(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass)
{
	if (UGameplayStatics::DoesSaveGameExist("SoundSettings", 0) == false)
	{
		if (UISMSettingsSave* SaveGameInstance = Cast<UISMSettingsSave>(UGameplayStatics::CreateSaveGameObject(SaveGameClass)))
		{
			BGMSlider->SetValue(SaveGameInstance->BGMValue);
			BGMValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(SaveGameInstance->BGMValue * 100)) + TEXT("%")));

			SFXSlider->SetValue(SaveGameInstance->SFXValue);
			SFXValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(SaveGameInstance->SFXValue * 100)) + TEXT("%")));

			UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SoundSettings", 0);
		}
	} // Don't have Sound Settings

	if (UISMSettingsSave* SaveGameInstance = Cast<UISMSettingsSave>(UGameplayStatics::LoadGameFromSlot("SoundSettings", 0)))
	{
		BGMSlider->SetValue(SaveGameInstance->BGMValue);
		ChangeSound(InSoundMix, InSoundBGMClass, SaveGameInstance->BGMValue, "BGM");

		SFXSlider->SetValue(SaveGameInstance->SFXValue);
		ChangeSound(InSoundMix, InSoundSFXClass, SaveGameInstance->SFXValue, "SFX");
	} // Set Sound Settings
}

void UISMOptionPanel::ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundClass, float Volume, FString Mode)
{
	if (UISMSettingsSave* SaveGameInstance = Cast<UISMSettingsSave>(UGameplayStatics::LoadGameFromSlot("SoundSettings", 0)))
	{
		if (Mode == "BGM")
		{
			SaveGameInstance->BGMValue = Volume;
			BGMValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Volume * 100)) + TEXT("%")));
		}
		else
		{
			SaveGameInstance->SFXValue = Volume;
			SFXValue->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Volume * 100)) + TEXT("%")));
		}

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "SoundSettings", 0))
		{
			UGameplayStatics::SetSoundMixClassOverride(GetWorld(), InSoundMix, InSoundClass, Volume, 1, 0);
		} // Save Sound Settings
	}
}

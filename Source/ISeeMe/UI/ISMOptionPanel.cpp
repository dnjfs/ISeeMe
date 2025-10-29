// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMOptionPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISeeMeCharacter.h"
#include <ISeeMe/ISMSettingsSave.h>
#include "Components/Button.h"
#include "Styling/SlateBrush.h"
#include "ISeeMe/ISMPlayerController.h"


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

void UISMOptionPanel::SetNetworkSettingUI()
{
	FButtonStyle OnButtonStyle = SmoothingOnButton->GetStyle();
	FButtonStyle OffButtonStyle = SmoothingOffButton->GetStyle();

	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		if (GI->bEnableSmoothCharacterMovement && RedButtonTexture && GreenButtonTexture) // Smoothing On
		{
			OnButtonStyle.Normal.SetResourceObject(RedButtonTexture);
			OnButtonStyle.Hovered.SetResourceObject(RedButtonTexture);
			OnButtonStyle.Pressed.SetResourceObject(RedButtonTexture);

			OffButtonStyle.Normal.SetResourceObject(GreenButtonTexture);
			OffButtonStyle.Hovered.SetResourceObject(GreenButtonTexture);
			OffButtonStyle.Pressed.SetResourceObject(GreenButtonTexture);
		}
		else // Smoothing Off
		{
			OnButtonStyle.Normal.SetResourceObject(GreenButtonTexture);
			OnButtonStyle.Hovered.SetResourceObject(GreenButtonTexture);
			OnButtonStyle.Pressed.SetResourceObject(GreenButtonTexture);
			
			OffButtonStyle.Normal.SetResourceObject(RedButtonTexture);
			OffButtonStyle.Hovered.SetResourceObject(RedButtonTexture);
			OffButtonStyle.Pressed.SetResourceObject(RedButtonTexture);
		}

		SmoothingOnButton->SetStyle(OnButtonStyle);
		SmoothingOffButton->SetStyle(OffButtonStyle);
	}
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

void UISMOptionPanel::SetSmoothCharacterMovement(bool bEnable)
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->bEnableSmoothCharacterMovement = bEnable;

		SetNetworkSettingUI();

		if (APlayerController* PlayerController = GI->GetFirstLocalPlayerController())
		{
			if (AISMPlayerController* ISMPlayerController = Cast<AISMPlayerController>(PlayerController))
			{
				if (AISeeMeCharacter* OtherCharacter = ISMPlayerController->GetOtherCharacter())
				{
					OtherCharacter->SetSmoothCharacterMovement(bEnable);
				}
			}
		}
	}
}
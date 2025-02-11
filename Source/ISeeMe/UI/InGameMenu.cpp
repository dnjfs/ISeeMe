// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/InGameMenu.h"
#include "ISeeMe/UI/ISMHUD.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include <ISeeMe/ISMSettingsSave.h>

void UInGameMenu::InitSoundSetting(TSubclassOf<USaveGame> SaveGameClass, USoundMix* InSoundMix, USoundClass* InSoundBGMClass, USoundClass* InSoundSFXClass)
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

void UInGameMenu::ChangeSound(USoundMix* InSoundMix, USoundClass* InSoundClass, float Volume, FString Mode)
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

void UInGameMenu::OnBackButtonPressed()
{
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			ToggleWidget(PlayerController);
		}
	}
}

void UInGameMenu::OnGuideButtonPressed()
{
}

void UInGameMenu::OnOptionButtonPressed()
{
}

void UInGameMenu::OnGraphicOptionButtonPressed()
{
	
}

void UInGameMenu::OnSoundOptionButtonPressed()
{

}

void UInGameMenu::OnControlButtonPressed()
{
}

void UInGameMenu::OnExitButtonPressed()
{
	//if (IOnlineSubsystem* OSS = IOnlineSubsystem::Get())
	//{
	//	IOnlineSessionPtr OnlineSessionPtr = OSS->GetSessionInterface();
	//	if (OnlineSessionPtr.IsValid())
	//	{
	//		OnlineSessionPtr->DestroySession(NAME_GameSession);
	//	}
	//}
	//if (UWorld* World = GetWorld())
	//{
	//	UGameplayStatics::OpenLevel(World, TEXT("EntryMap"));
	//}

	GetGameInstance()->ReturnToMainMenu();
}

void UInGameMenu::ToggleWidget(APlayerController* PlayerController)
{
	if (GetIsEnabled() == false)
	{
		SetIsEnabled(true);
		SetVisibility(ESlateVisibility::Visible);

		PlayerController->SetIgnoreLookInput(true);
		PlayerController->SetIgnoreMoveInput(true);

		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;

		PlayerController->SetInputMode(FInputModeGameAndUI());

	}
	else
	{
		SetIsEnabled(false);
		SetVisibility(ESlateVisibility::Hidden);

		PlayerController->SetIgnoreLookInput(false);
		PlayerController->SetIgnoreMoveInput(false);

		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;

		PlayerController->SetInputMode(FInputModeGameOnly());

	}
}

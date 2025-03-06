// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/InGameMenu.h"
#include "ISeeMe/UI/ISMHUD.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include <ISeeMe/ISMSettingsSave.h>



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

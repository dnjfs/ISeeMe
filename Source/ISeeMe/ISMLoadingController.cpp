// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLoadingController.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>

AISMLoadingController::AISMLoadingController()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)) // 추가된 부분
{
}

void AISMLoadingController::BeginPlay()
{
	Super::BeginPlay();

	if (UILoadingClass == nullptr)
		return;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			UILoadingInstance = CreateWidget<UUserWidget>(PC, UILoadingClass);
			if (UILoadingInstance)
			{
				UILoadingInstance->AddToViewport(1);
				UILoadingInstance->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempted to create widget on non-local controller!"));
		}
	}

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UILoadingInstance->GetCachedWidget());
	SetInputMode(Mode);
	SetShowMouseCursor(true);

	CreateSession("EntryMap");
}

void AISMLoadingController::CreateSession(FName ChapterName)
{
	if (UILoadingInstance)
		UILoadingInstance->SetVisibility(ESlateVisibility::Visible);

	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = Subsystem->GetSessionInterface();

		if (OnlineSessionInterface.IsValid() == false)
		{
			LOG_SCREEN("Session Interface is Invalid");
			return;
		}

		// Check Origin Session
		if (FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(ChapterName))
		{
			LOG_SCREEN("Destroying Existing Session...");
			OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
			OnlineSessionInterface->DestroySession(ChapterName);
		}
		else
		{
			StartCreatingSession(ChapterName);
		}
	}
}

void AISMLoadingController::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LOG_SCREEN("Successfully destroyed session: %s", *SessionName.ToString());
		StartCreatingSession(SessionName);  
	}
	else
	{
		LOG_SCREEN("Failed to destroy session");
	}
}

// Real Create Session
void AISMLoadingController::StartCreatingSession(FName ChapterName)
{
	LOG_SCREEN("Creating New Session...");

	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = 2;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinInProgress = true;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		SessionSettings->bIsLANMatch = true;
	}
	else
	{
		SessionSettings->bIsLANMatch = false;
	}
	SessionSettings->bIsDedicated = false;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;

	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), ChapterName, *SessionSettings);
}

void AISMLoadingController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		LOG_SCREEN("Failed CreateSession()");
		return;
	}

	LOG_SCREEN("Successful CreateSession() - %s", *SessionName.ToString());

	UGameplayStatics::OpenLevel(this, SessionName, true, "Listen");
}

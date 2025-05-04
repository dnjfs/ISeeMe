// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyController.h"

#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"
#include <Net/UnrealNetwork.h>
#include "UI/ISMLobbyMenu.h"
#include "UI/ISMCharacterSelect.h"
#include "ISMLobbyGameMode.h"
#include "ISMLobbyGameState.h"
#include "ISMGameInstance.h"


AISMLobbyController::AISMLobbyController()
	: FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplate))
	, FindFriendSessionCompleteDelegate(FOnFindFriendSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnFindFriendSessionComplete))
	, SessionUserInviteAcceptedDelegate(FOnSessionUserInviteAcceptedDelegate::CreateUObject(this, &ThisClass::OnSessionUserInviteAccepted))
	, DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
{
}

void AISMLobbyController::BeginPlay()
{
	Super::BeginPlay();

	InitUI();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	SetShowMouseCursor(true);

	if (GetSessionInterface() == false)
	{
		LOG_SCREEN("Failed GetSessionInterface()");
		return;
	}

	if (!HasAuthority())
	{
		UIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
		UIWidgetInstance->ChangeLobbyUI(3);
	} // Show Select Chpater UI after join client
}

void AISMLobbyController::InitUI()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (UIWidgetClass)
		{
			UIWidgetInstance = CreateWidget<UISMLobbyMenu>(PC, UIWidgetClass);
			if (UIWidgetInstance && IsLocalPlayerController())
			{
				UIWidgetInstance->AddToViewport();
				UIWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to create widget on non-local controller!"));
	}
}

void AISMLobbyController::ControllerChangeLobbyUI(int32 Index)
{
	UIWidgetInstance->ChangeLobbyUI(Index);
} // Change Lobby UI

void AISMLobbyController::MulticastControllerChangeUI_Implementation(int32 Index)
{
	UIWidgetInstance->ChangeLobbyUI(Index);
} // Multicast Change Chapter UI After Select Chapter

// When BackButto is Clicked
void AISMLobbyController::CallBackUI(const FString& Name)
{
	if (!HasAuthority())
	{
		ServerBackUI(Name);
	}
	else
	{
		if (Name == "Chapter")
		{
			if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->BackChapterUI();
			}
		} // From Select Character UI To Select Chapter UI
		else
		{
			UGameplayStatics::OpenLevel(this, "LoadingMap");
		} // From Select Chapter UI To Lobby UI (But Level is LoadingMap)
	}
}

void AISMLobbyController::ServerBackUI_Implementation(const FString& Name)
{
	if (Name == "Chapter")
	{
		if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->BackChapterUI();
		}
	}
	else
	{
		UGameplayStatics::OpenLevel(this, "LoadingMap");
	}
}

// Show Select Character UI
void AISMLobbyController::CallSelectCharacterUI()
{
	ControllerChangeLobbyUI(4);

	if (HasAuthority())
		ClientSelectCharacterUI();
}

void AISMLobbyController::ClientSelectCharacterUI_Implementation()
{
	ControllerChangeLobbyUI(4);
}


// Show Select Character Buttons
void AISMLobbyController::CallChangeCharacterButton(FString CharacterSelect)
{
	if (!HasAuthority())
	{
		ServerChangeCharacterButton(CharacterSelect);
	}
	else
	{
		if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			// Host
			GM->ChangeCharacterButton(CharacterSelect,0); 
		}
	}
}

void AISMLobbyController::ServerChangeCharacterButton_Implementation(const FString& CharacterSelect)
{
	if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		// Client
		GM->ChangeCharacterButton(CharacterSelect, 1);
	}
}

// Multicast Change Character Buttons
void AISMLobbyController::MulticastChangeCharacterButton_Implementation(const FString& CharacterSelect, FLinearColor Color)
{
	UWidgetSwitcher* Switcher = UIWidgetInstance->LobbySwitcher;
	if (Switcher)
	{
		UWidget* ActiveWidget = Switcher->GetActiveWidget();
		if (ActiveWidget && ActiveWidget->IsA(UISMCharacterSelect::StaticClass()))
		{
			UISMCharacterSelect* CharacterSelectWidget = Cast<UISMCharacterSelect>(ActiveWidget);
			if (CharacterSelectWidget && IsLocalController())
			{				
				if (CharacterSelect == "Hojin")
				{
					CharacterSelectWidget->HojinButton->SetBackgroundColor(Color);
				}
				else if (CharacterSelect == "Mimi")
				{
					CharacterSelectWidget->MimiButton->SetBackgroundColor(Color);
				}
				else if (CharacterSelect == "None")
				{
					CharacterSelectWidget->InitButton();
				}
			}

			if (HasAuthority())
			{
				CharacterSelectWidget->VisibleApply();
			}
		}
	}
}

void AISMLobbyController::ExitGame()
{
	if (UWorld* World = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(World, this, EQuitPreference::Quit, true);
	}
}

// Show Tutorial Widget
void AISMLobbyController::CallTutorial()
{
	ControllerChangeLobbyUI(5);

	if (HasAuthority())
	{
		ClientTutorial();
	}
}

void AISMLobbyController::ClientTutorial_Implementation()
{
	ControllerChangeLobbyUI(5);
}

// Check Reading Tutorial
void AISMLobbyController::CallReadTutorial()
{
	if (!bDoneRead)
	{
		bDoneRead = true;
		if (HasAuthority())
		{
			ReadTutorial();
		}
		else
		{
			ServerReadTutorial();
		}
	}
}

void AISMLobbyController::ReadTutorial()
{
	if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->CountReadTutorial();
	}
}

void AISMLobbyController::ServerReadTutorial_Implementation()
{
	ReadTutorial();
}


//////////////////////////////////////////////////////////////////////////////////////////////////

// Online Session
bool AISMLobbyController::GetSessionInterface()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS == nullptr)
		return false;
	
	LOG_SCREEN("SubsystemName: %s", *OSS->GetSubsystemName().ToString());

	OnlineSessionInterface = OSS->GetSessionInterface();
	if (OnlineSessionInterface.IsValid() == false)
		return false;


	// 델리게이트 등록
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		OnlineSessionInterface->AddOnFindFriendSessionCompleteDelegate_Handle(LocalPlayer->GetControllerId(), FindFriendSessionCompleteDelegate);
	else
		return false;
	OnlineSessionInterface->AddOnSessionUserInviteAcceptedDelegate_Handle(SessionUserInviteAcceptedDelegate);

	return true;
}

void AISMLobbyController::FindSession()
{
	if (UILoadingInstance)
		UILoadingInstance->SetVisibility(ESlateVisibility::Visible); // Loading Screen

	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	// 델리게이트 연결
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Find Game Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 100;	// 검색 결과로 나오는 세션 수 최대값
	SessionSearch->bIsLanQuery = false;		// LAN 사용 여부
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 Presence로 설정

	// 세션 검색
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AISMLobbyController::OnFindSessionComplete(bool bWasSuccessful)
{
	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
			return;
	}

	if (!bWasSuccessful)
	{
		LOG_SCREEN("Failed FindSession()");
		return;
	}

	LOG_SCREEN("======== Search Result ========");

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		// 매치 타입 확인하기
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// 찾은 세션의 정보 출력하기
		LOG_SCREEN("Session ID: %s / Owner: %s", *Id, *User);

		// 세션의 매치 타입이 "FreeForAll"일 경우 세션 참가
		if (MatchType == FString("FreeForAll"))
		{
			LOG_SCREEN("Joining Match Type: %s", *MatchType);

			CachedInviteResult = Result;
			if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
			{
				LOG_SCREEN("Existing Session Found - Destroying...");
				OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
				OnlineSessionInterface->DestroySession(NAME_GameSession);
			}
			else
			{
				OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
				// 세션 참가
				if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
					OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
			}
		}
	}
}

void AISMLobbyController::OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResult)
{
	LOG_SCREEN("OnFindFriendSessionComplete called");

	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}
		
	if (!bWasSuccessful)
	{
		LOG_SCREEN("Failed to find friend session");
		return;
	}

	LOG_SCREEN("Before %d", SearchResult.Num());

	FName SessionName = NAME_GameSession;

	// 세션이 존재하는지 확인 후 삭제
	if (OnlineSessionInterface->GetNamedSession(SessionName))
	{
		OnlineSessionInterface->DestroySession(SessionName);
		LOG_SCREEN("Session Destroyed: %s", *SessionName.ToString());
	}
	else
	{
		LOG_SCREEN("No Session Found to Destroy.");
	}

	LOG_SCREEN("After %d", SearchResult.Num());

	LOG_SCREEN("======== Search Result ========");

	check(SearchResult.Num() == 1); // 세션이 1개여야 정상

	CachedInviteResult = SearchResult[0];
	if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		LOG_SCREEN("Existing Session Found - Destroying...");
		OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}
	else
	{
		// 세션 참가
		JoinSession(CachedInviteResult);
	}
}

void AISMLobbyController::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	LOG_SCREEN("OnSessionUserInviteAccepted called");

	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	if (!bWasSuccessful)
	{
		LOG_SCREEN("Failed to find friend session");
		return;
	}

	CachedInviteResult = InviteResult;
	if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		LOG_SCREEN("Existing Session Found - Destroying...");
		OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}
	else
	{
		// 세션 참가
		JoinSession(CachedInviteResult);
	}
}

void AISMLobbyController::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		LOG_SCREEN("Successfully destroyed session: %s", *SessionName.ToString());
		JoinSession(CachedInviteResult);
	}
	else
	{
		LOG_SCREEN("Failed to destroy session");
	}
}

void AISMLobbyController::JoinSession(const FOnlineSessionSearchResult& Result)
{
	if (!Result.IsValid())
	{
		LOG_SCREEN("Session is invalid");
		return;
	}

	// 델리게이트 연결
	OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	// 세션 참가
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
}

void AISMLobbyController::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Type::Success)
	{
		LOG_SCREEN("Failed JoinSession() - %d", Result);
		return;
	}

	// 세션에 정상적으로 참가하면 IP Address 얻어와서 해당 서버에 접속
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		LOG_SCREEN("IP Address: %s", *Address);

		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}

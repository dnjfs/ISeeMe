// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyController.h"

#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Kismet/GameplayStatics.h"

AISMLobbyController::AISMLobbyController()
	: CreateSessionComplete(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
	, FindSessionComplete(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete))
	, JoinSessionComplete(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplate))
{
}

void AISMLobbyController::BeginPlay()
{
	Super::BeginPlay();

	if (UIWidgetClass == nullptr)
		return;

	UIWidgetInstance = CreateWidget<UUserWidget>(this, UIWidgetClass);
	if (UIWidgetInstance == nullptr)
		return;

	UIWidgetInstance->AddToViewport();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIWidgetInstance->GetCachedWidget());
	SetInputMode(Mode);
	SetShowMouseCursor(true);

	if (GetSessionInterface() == false)
	{
		LOG_SCREEN("Failed GetSessionInterface()");
		return;
	}
}

bool AISMLobbyController::GetSessionInterface()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS == nullptr)
		return false;
	
	LOG_SCREEN("SubsystemName: %s", *OSS->GetSubsystemName().ToString());

	OnlineSessionInterface = OSS->GetSessionInterface();
	if (OnlineSessionInterface.IsValid() == false)
		return false;

	return true;
}

void AISMLobbyController::CreateSession()
{
	// 세션 인터페이스 유효성 검사
	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	// NAME_GameSession 이름의 세션이 존재하는지 검사하여 파괴
	if (FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
		LOG_SCREEN("Destroy session: %s", NAME_GameSession);
	}

	// 델리게이트 연결
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionComplete);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = 2;		// 허용되는 플레이어 수
	SessionSettings->bShouldAdvertise = true;		// 광고되는 세션인지 개인 세션인지
	SessionSettings->bAllowJoinInProgress = true;	// 세션 진행중에 참여 허용
	SessionSettings->bIsLANMatch = false;			// LAN에서만 실행되고 외부 공개되지 않음
	SessionSettings->bIsDedicated = false;			// 데디케이티드 서버인지 (리슨 서버가 아닌지)
	SessionSettings->bUsesPresence = true;			// Presence 사용 (유저 정보에 세션 정보를 표시하는듯)
	SessionSettings->bAllowJoinViaPresence = true;	// Presence를 통해 참여 허용
	SessionSettings->bUseLobbiesIfAvailable = true; // 플랫폼이 지원하는 경우 로비 API 사용

	// FOnlineSessionSettings() 코드 참고
	// 세션의 MatchType을 모두에게 열림, 온라인 서비스와 핑 데이터를 통해 세션 홍보 옵션으로 설정
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 세션 생성
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AISMLobbyController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		LOG_SCREEN("Failed CreateSession()");
		return;
	}

	LOG_SCREEN("Successful CreateSession()");
	
	UGameplayStatics::OpenLevel(this, TEXT("ThirdPersonMap"), true, "Listen");
}

void AISMLobbyController::JoinSession()
{
	if (OnlineSessionInterface.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	// 델리게이트 연결
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionComplete);

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

			// 델리게이트 연결
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionComplete);

			// 세션 참가
			if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
				OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
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
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
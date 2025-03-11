// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLoadingController.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>

AISMLoadingController::AISMLoadingController()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
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
		UILoadingInstance->SetVisibility(ESlateVisibility::Visible); // Loading Screen

	if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = Subsystem->GetSessionInterface();

		// 세션 인터페이스 유효성 검사
		if (OnlineSessionInterface.IsValid() == false)
		{
			LOG_SCREEN("Session Interface is Invalid");
			return;
		}

		// NAME_GameSession 이름의 세션이 존재하는지 검사하여 파괴
		if (FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(ChapterName))
		{
			OnlineSessionInterface->DestroySession(ChapterName);
			//LOG_SCREEN("Destroy session: %s", NAME_GameSession);
		}

		// 델리게이트 연결
		OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

		TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
		SessionSettings->NumPublicConnections = 2;		// 허용되는 플레이어 수
		SessionSettings->bShouldAdvertise = true;		// 광고되는 세션인지 개인 세션인지
		SessionSettings->bAllowJoinInProgress = true;	// 세션 진행중에 참여 허용
		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			SessionSettings->bIsLANMatch = true;
		}
		else
		{
			SessionSettings->bIsLANMatch = false;
		}
		SessionSettings->bIsDedicated = false;			// 데디케이티드 서버인지 (리슨 서버가 아닌지)
		SessionSettings->bUsesPresence = true;			// Presence 사용 (유저 정보에 세션 정보를 표시하는듯)
		SessionSettings->bAllowJoinViaPresence = true;	// Presence를 통해 참여 허용
		SessionSettings->bUseLobbiesIfAvailable = true; // 플랫폼이 지원하는 경우 로비 API 사용
		SessionSettings->bUseLobbiesVoiceChatIfAvailable = true;  // 음성 채팅 사용

		// FOnlineSessionSettings() 코드 참고
		// 세션의 MatchType을 모두에게 열림, 온라인 서비스와 핑 데이터를 통해 세션 홍보 옵션으로 설정
		SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		// 세션 생성
		if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
			OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), ChapterName, *SessionSettings);
	}
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
// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyController.h"

#include "Blueprint/UserWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

AISMLobbyController::AISMLobbyController()
	: CreateSessionComplete(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
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
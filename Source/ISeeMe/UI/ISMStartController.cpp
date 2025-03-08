// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMStartController.h"
#include "Blueprint/UserWidget.h"

#include <ISeeMe/ISMLobbyController.h>
#include <ISeeMe/ISMLobbyGameMode.h>

void AISMStartController::BeginPlay()
{
	Super::BeginPlay();

	InitUI();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(UIFriendInviteInstance->GetCachedWidget());
	SetInputMode(Mode);
	SetShowMouseCursor(true);

	if (!HasAuthority())
	{
		UIChapterSelectClientInstance->SetVisibility(ESlateVisibility::Visible);
		ServerChangeChapterSelect();
	}
	else if(IsLocalPlayerController() && HasAuthority())
	{
		UIFriendInviteInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void AISMStartController::InitUI()
{
	LOG_SCREEN("Init");
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController())
		{
			CreateAndInitWidget(UIFriendInviteInstance, UIFriendInviteClass, 0, PC);
			CreateAndInitWidget(UIChapterSelectServerInstance, UIChapterSelectServerClass, 1, PC);
			CreateAndInitWidget(UIChapterSelectClientInstance, UIChapterSelectClientClass, 1, PC);
			CreateAndInitWidget(UICharacterSelectInstance, UICharacterSelectClass, 2, PC);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Attempted to create widget on non-local controller!"));
		}
	}
}

void AISMStartController::CreateAndInitWidget(TObjectPtr<UUserWidget>& WidgetInstance, TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder, APlayerController* PC)
{
	if (WidgetClass)
	{
		WidgetInstance = CreateWidget<UUserWidget>(PC, WidgetClass);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport(ZOrder);
			WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AISMStartController::ServerChangeChapterSelect_Implementation()
{
	UIFriendInviteInstance->SetVisibility(ESlateVisibility::Hidden);
	UIChapterSelectServerInstance->SetVisibility(ESlateVisibility::Visible);
}

void AISMStartController::CallSelectCharacterUI()
{
	SelectCharacterUI();

	if (HasAuthority())
		ClientSelectCharacterUI();
}

void AISMStartController::ClientSelectCharacterUI_Implementation()
{
	SelectCharacterUI();
}

void AISMStartController::SelectCharacterUI()
{
	UIChapterSelectServerInstance->SetVisibility(ESlateVisibility::Hidden);
	UIChapterSelectClientInstance->SetVisibility(ESlateVisibility::Hidden);
	UICharacterSelectInstance->SetVisibility(ESlateVisibility::Visible);
}
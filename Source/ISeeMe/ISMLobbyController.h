// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ISMLobbyController.generated.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

//DECLARE_DELEGATE_TwoParams(FOnCreateSessionCompleteDelegate, FName /*SessionName*/, bool /*bWasSuccessful*/);
DECLARE_DELEGATE_OneParam(FOnFindSessionsCompleteDelegate, bool /*bWasSuccessful*/);
DECLARE_DELEGATE_TwoParams(FOnJoinSessionCompleteDelegate, FName /*SessionName*/, EOnJoinSessionCompleteResult::Type /*Result*/);

/**
 * 
 */
UCLASS()
class ISEEME_API AISMLobbyController : public APlayerController
{
	GENERATED_BODY()

	AISMLobbyController();

public:
	/*UFUNCTION(BlueprintCallable)
	void CreateSession(FName ChapterName);*/

	UFUNCTION()
	void CallSelectChapterUI();

	UFUNCTION()
	void ClientSelectChapterUI();

	UFUNCTION()
	void LobbyUI();

	UFUNCTION()
	void InitUI();

	UFUNCTION()
	void CallSelectCharacterUI();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Unreliable)
	void ClientSelectCharacterUI();

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UISMLobbyMenu> UIWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UILoadingClass;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIFriendInviteClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIChapterServerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIChapterClientClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UICharacterSelectClass;*/

private:
	UPROPERTY()
	TObjectPtr<class UISMLobbyMenu> UIWidgetInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UILoadingInstance;

	/*UPROPERTY()
	TObjectPtr<class UUserWidget> UIFriendInviteInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UIChapterServerInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UIChapterClientInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UICharacterSelectInstance;*/

	UFUNCTION()
	void SelectCharacterUI();

	void CreateAndInitWidget(TObjectPtr<UUserWidget>& WidgetInstance, TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder, APlayerController* PC);

// Online Subsystem
protected:
	bool GetSessionInterface();

	UFUNCTION(BlueprintCallable)
	void FindSession();
	UFUNCTION(BlueprintCallable)
	void ExitGame();
	
	/*void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);*/
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	void OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>&);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void JoinSession(const FOnlineSessionSearchResult& Result);

private:
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	//FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	FOnFindFriendSessionCompleteDelegate FindFriendSessionCompleteDelegate;
	FOnSessionUserInviteAcceptedDelegate SessionUserInviteAcceptedDelegate;
};

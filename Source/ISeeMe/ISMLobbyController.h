// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
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
	UFUNCTION()
	void InitUI();

	UFUNCTION(BlueprintCallable)
	void ControllerChangeLobbyUI(int32 Index); // Change Lobby UI (Switcher UI)

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MulticastControllerChangeUI(int32 Index); // Multicast Change Lobby UI (Switcher UI)

	UFUNCTION()
	void CallSelectCharacterUI(); // Call Change Character UI

	UFUNCTION(BlueprintCallable)
	void CallChangeCharacterButton(FString CharacterSelect); // Call Change Character Button

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerChangeCharacterButton(const FString& CharacterSelect);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void MulticastChangeCharacterButton(const FString& CharacterSelect, FLinearColor Color);

	/// ///////////////////////////////////////////////////

	UPROPERTY()
	TObjectPtr<class UISMLobbyMenu> UIWidgetInstance;

	UFUNCTION(BlueprintCallable)
	void CallBackUI(const FString& Name);


protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Unreliable)
	void ClientSelectCharacterUI(); // Change Character UI in Client

	UFUNCTION(Server,Reliable)
	void ServerBackUI(const FString& Name);

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UISMLobbyMenu> UIWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UILoadingClass;

	// Online Subsystem
	bool GetSessionInterface();

	UFUNCTION(BlueprintCallable)
	void FindSession();
	UFUNCTION(BlueprintCallable)
	void ExitGame();
	
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	void OnFindFriendSessionComplete(int32 LocalUserNum, bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>&);
	void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);
	void JoinSession(const FOnlineSessionSearchResult& Result);

private:
	UPROPERTY()
	TObjectPtr<class UUserWidget> UILoadingInstance;

	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	FOnFindSessionsCompleteDelegate FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	FOnFindFriendSessionCompleteDelegate FindFriendSessionCompleteDelegate;
	FOnSessionUserInviteAcceptedDelegate SessionUserInviteAcceptedDelegate;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISMStartController.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMStartController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void CallSelectCharacterUI();

	void InitUI();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void ServerChangeChapterSelect();

	UFUNCTION(Client, Unreliable)
	void ClientSelectCharacterUI();

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIFriendInviteClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIChapterSelectServerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIChapterSelectClientClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UICharacterSelectClass;

private:
	UPROPERTY()
	TObjectPtr<class UUserWidget> UIFriendInviteInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UIChapterSelectServerInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UIChapterSelectClientInstance;

	UPROPERTY()
	TObjectPtr<class UUserWidget> UICharacterSelectInstance;

	UFUNCTION()
	void SelectCharacterUI();

	void CreateAndInitWidget(TObjectPtr<UUserWidget>& WidgetInstance, TSubclassOf<UUserWidget> WidgetClass, int32 ZOrder, APlayerController* PC);
	
};

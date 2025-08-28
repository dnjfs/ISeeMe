// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ISMGameInstance.generated.h"

class UISMSaveGame;

UCLASS()
class ISEEME_API UISMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // GI에 저장된 데이터를 로컬 SaveGame으로 저장
    void SaveGame();

    // 로컬 SaveGame에 저장된 데이터를 GI로 불러오기
    void LoadGame(UISMSaveGame* LoadedGame);

    UFUNCTION(BlueprintCallable)
    void SetSelectedPawnClass(TSubclassOf<ACharacter> ServerCharacter, TSubclassOf<ACharacter> ClientCharacter);

    UPROPERTY(Transient)
    TSubclassOf<APawn> SelectedPawnClass;

    UPROPERTY(Transient)
    TSubclassOf<APawn> ClientPawnClass;

    int CurrChapterNo;

    // 클리어한 최대 챕터
    int MaxChapterNo;

    // Have been Tutorial?
    bool bTutorial = false;

    FName SavedCheckPointID;

    TArray<FName> AcquiredRecollectionIDs;

    // 첫 실행 여부
    bool bIsFirstLaunch = false;
};

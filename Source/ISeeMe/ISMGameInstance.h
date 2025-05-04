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

    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<APawn> SelectedPawnClass;

    UPROPERTY(BlueprintReadWrite)
    int CurrChapterNo;

    // 클리어한 최대 챕터
    UPROPERTY(BlueprintReadWrite)
    int MaxChapterNo;

    // Have been Tutorial?
    UPROPERTY(BlueprintReadWrite)
    bool bTutorial = false;

    FName SavedCheckPointID;
};

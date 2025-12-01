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
    virtual void Init() override;

    // GI に保存されたデータをローカル SaveGame に保存
    void SaveGame();

    // ローカル SaveGame に保存されたデータを GI に読み込む
    void LoadGame(UISMSaveGame* LoadedGame);

    UFUNCTION(BlueprintCallable)
    void SetSelectedPawnClass(TSubclassOf<ACharacter> ServerCharacter, TSubclassOf<ACharacter> ClientCharacter);

    UPROPERTY(Transient)
    TSubclassOf<APawn> SelectedPawnClass;

    UPROPERTY(Transient)
    TSubclassOf<APawn> ClientPawnClass;

    int CurrChapterNo;

    // クリア済みの最大チャプター
    int MaxChapterNo;

    // Have been Tutorial?
    bool bTutorial = false;

    FName SavedCheckPointID;

    TArray<FName> AcquiredRecollectionIDs;

    // 初回起動フラグ
    bool bIsFirstLaunch = false;

    void SoundPlay(USoundBase* Sound);

    bool bEnableSmoothCharacterMovement = false;

private:
    UPROPERTY(Transient)
    TSubclassOf<ACharacter> PreloadedCharacterClass1;

    UPROPERTY(Transient)
    TSubclassOf<ACharacter> PreloadedCharacterClass2;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ISMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadWrite)
    TSubclassOf<APawn> SelectedPawnClass;

    UPROPERTY(BlueprintReadWrite)
    int CurrChapterNo;

    FName SavedCheckPointID;
};

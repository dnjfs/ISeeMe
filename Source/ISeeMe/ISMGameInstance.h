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
	UPROPERTY()
	TArray<TSubclassOf<APawn>> SelectedPawnClasses;

    UPROPERTY()
    TSubclassOf<APawn> SelectedPawnClass;

    /*void AddToPersistentArray(const TSubclassOf<APawn>& NewValue);

    const TArray<TSubclassOf<APawn>>& GetPersistentArray() const
    {
        return SelectedPawnClasses;
    }*/

    int CurrChapterNo;

    FName SavedCheckPointID;
};

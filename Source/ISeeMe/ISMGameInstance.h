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
	/*Player Pawn*/
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<APawn> SelectedPawnClass;


	UFUNCTION(BlueprintCallable)
	void SetSelectedPawnClass(TSubclassOf<APawn> NewPawnClass);
};

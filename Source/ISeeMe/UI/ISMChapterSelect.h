// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMChapterSelect.generated.h"

class UWrapBox;

/**
 * 
 */
UCLASS()
class ISEEME_API UISMChapterSelect : public UUserWidget
{
	GENERATED_BODY()

public:
	void EnableChapterSelectButton(int MaxChapterNo);

protected:
	
private:
	UFUNCTION(BlueprintCallable)
	void SelectChapter(int ChapterNo);

	UPROPERTY(meta = (BindWidget))
	UWrapBox* ChapterListBox;
};

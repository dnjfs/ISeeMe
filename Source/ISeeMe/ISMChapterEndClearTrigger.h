// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMChapterClearTrigger.h"
#include "ISMChapterEndClearTrigger.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(OnClearEndUpdated, bool);

UCLASS()
class ISEEME_API AISMChapterEndClearTrigger : public AISMChapterClearTrigger
{
	GENERATED_BODY()
	
public:
	FOnClearUpdated OnClearEndUpdated;
protected:
	void CompleteChapter() override;
};

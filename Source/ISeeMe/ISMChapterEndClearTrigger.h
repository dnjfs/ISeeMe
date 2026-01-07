// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMChapterClearTrigger.h"
#include "ISMChapterEndClearTrigger.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FOnClearEndUpdated);

UCLASS()
class ISEEME_API AISMChapterEndClearTrigger : public AISMChapterClearTrigger
{
	GENERATED_BODY()
	
public:
	FOnClearEndUpdated OnClearEndUpdated;

protected:
	void CompleteChapter() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowEpilogue();
};

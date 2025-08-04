// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnlineStats.h"
#include "AchievementManager.generated.h"

UCLASS()
class ISEEME_API UAchievementManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UAchievementManager();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);
	void UpdateAchievementProgress(const FString& Id, float Percent);

	FOnlineAchievementsWritePtr AchievementsWriteObjectPtr;
};

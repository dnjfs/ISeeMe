// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ISEEME_API AchievementManager
{
public:
	AchievementManager();

	void QueryAchievements();
	void OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful);

	void UpdateAchievementProgress(const FString& Id, float Percent);

	/** The object we're going to use in order to progress any achievement */
	FOnlineAchievementsWritePtr AchievementsWriteObjectPtr;

	~AchievementManager();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "AchievementManager.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "OnlineSubsystem.h"
#include "Engine/LocalPlayer.h"

// Sets default values
UAchievementManager::UAchievementManager()
{

}


void UAchievementManager::Initialize(FSubsystemCollectionBase& Collection)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 30.f, FColor::Cyan, TEXT("UAchievementManager::Initialize()"));
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();

			if (Achievements.IsValid())
			{
				//Cache all the game's achievements for future use and bind the OnQueryAchievementsComplete function to fire when we're finished caching
				Achievements->QueryAchievements(*UserId.Get(), FOnQueryAchievementsCompleteDelegate::CreateUObject(this, &UAchievementManager::OnQueryAchievementsComplete));
			}
		}
	}
}

void UAchievementManager::OnQueryAchievementsComplete(const FUniqueNetId& PlayerId, const bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GLog->Log("achievements were cached");
	}
	else
	{
		GLog->Log("failed to cache achievements");
	}
}

void UAchievementManager::UpdateAchievementProgress(const FString& Id, float Percent)
{
	//Get the online sub system
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		//Get the Identity from the sub system 
		//Meaning our player's profile and various online services
		IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();

		if (Identity.IsValid())
		{
			//Get a thread-safe pointer (for more info check out this link: https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Templates/TSharedPtr/index.html )
			TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);

			//Get the achievements interface for this platform
			IOnlineAchievementsPtr Achievements = OnlineSub->GetAchievementsInterface();


			if (Achievements.IsValid() && (!AchievementsWriteObjectPtr.IsValid() || !AchievementsWriteObjectPtr->WriteState != EOnlineAsyncTaskState::InProgress))
			{
				//Make a shared pointer for achievement writing
				AchievementsWriteObjectPtr = MakeShareable(new FOnlineAchievementsWrite());

				//Sets the progress of the desired achievement - does nothing if the id is not valid
				AchievementsWriteObjectPtr->SetFloatStat(Id, Percent);

				//Write the achievements progress
				FOnlineAchievementsWriteRef AchievementsWriteObjectRef = AchievementsWriteObjectPtr.ToSharedRef();
				Achievements->WriteAchievements(*UserId, AchievementsWriteObjectRef);
			}
		}
	}
}

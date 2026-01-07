// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterClearTrigger.h"
#include "Components/BoxComponent.h"
#include "ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISMGameInstance.h"
#include "GameFramework/Character.h"
#include "ISMPlayerController.h"
#include "ISeeMe/UI/ISMOverlay.h"
#include "Components/WidgetSwitcher.h" 
#include <GameFramework/HUD.h>
#include "ISeeMe/UI/ISMHUD.h"
#include "ISMLobbyController.h"
#include <Net/UnrealNetwork.h>


// Sets default values
AISMChapterClearTrigger::AISMChapterClearTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	// Multicast RPC를 위해 액터 리플리케이트 활성화
	SetReplicates(true);
	// 오너가 없는 액터이므로 오너에게만 연관성이 있으면 안됨
	bOnlyRelevantToOwner = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
}

// Called when the game starts or when spawned
void AISMChapterClearTrigger::BeginPlay()
{
	Super::BeginPlay();

	DetectedPlayerCount = 0;

	if (HasAuthority())
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapBegin);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AISMChapterClearTrigger::OnOverlapEnd);
	}
}

void AISMChapterClearTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		DetectedPlayerCount++;
	}

	// When all detect
	if (DetectedPlayerCount == 2)
	{
		CompleteChapter();
	}
}

void AISMChapterClearTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		DetectedPlayerCount--;
	}
}

void AISMChapterClearTrigger::CompleteChapter()
{
	MulticastSaveChapterNo();
	MulticastPlaySound();
	MulticastShowLoadingWidget();
	MoveToNextChapter();
}

void AISMChapterClearTrigger::MulticastSaveChapterNo_Implementation()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->MaxChapterNo = FMath::Max(GI->MaxChapterNo, NextChapterNo);
		GI->CurrChapterNo = NextChapterNo;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();
	}
}

void AISMChapterClearTrigger::MulticastPlaySound_Implementation()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		GI->SoundPlay(ClearSound);
	}
}

void AISMChapterClearTrigger::MulticastShowLoadingWidget_Implementation()
{
	// Delegate Execute (Loading UI)
	OnClearUpdated.ExecuteIfBound(true);
}

void AISMChapterClearTrigger::MoveToNextChapter()
{
	UWorld* World = GetWorld(); 

	if (HasAuthority())
	{
		World->ServerTravel("/Game/ISeeMe/Maps/" + NextChapter + "?listen", true);
	}
}

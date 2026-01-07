// Fill out your copyright notice in the Description page of Project Settings.

#include "ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UISMGameInstance::Init()
{
	Super::Init();

	// [NOTE] 서버에서 스폰한 액터가 클라이언트에 복제되지 않는 문제에 대한 해결 코드
	// 1. 서버는 스폰할 액터의 원형(Archetype) 정보를 NetGUID를 통해 클라이언트로 전송한다.
	// 2. 클라이언트는 전송받은 NetGUID를 이용해 자신의 메모리에 로드된 UClass(Archetype)를 찾는다.
	// 3. 만약 SeamlessTravel 중 GC에 의해 해당 UClass에 대한 하드 레퍼런스가 모두 사라지면,
	//    UClass가 메모리에서 언로드되어 Archetype을 찾지 못하고 액터 스폰에 실패하게 된다.
	// 4. 이를 방지하기 위해, GameInstance에서 해당 클래스들을 직접 로드하여 하드 레퍼런스를 유지시킨다.
	//    이를 통해 GC가 UClass를 제거하는 것을 막고, 네트워크를 통한 캐릭터 스폰이 항상 가능하도록 보장한다.
	//
	// - 참고: UClass는 CDO를 포함하고 있고, 클라이언트 스폰 시엔 NetGUID로 찾은 UClass의 CDO를 통해 스폰
	PreloadedCharacterClass1 = LoadClass<ACharacter>(nullptr, TEXT("/Game/ISeeMe/Blueprints/BP_Boy.BP_Boy_C"));
	PreloadedCharacterClass2 = LoadClass<ACharacter>(nullptr, TEXT("/Game/ISeeMe/Blueprints/BP_Mimi.BP_Mimi_C"));

	check(PreloadedCharacterClass1 && PreloadedCharacterClass2);
}

void UISMGameInstance::SaveGame()
{
	if (UISMSaveGame* SaveGameInstance = Cast<UISMSaveGame>(UGameplayStatics::CreateSaveGameObject(UISMSaveGame::StaticClass())))
	{
		SaveGameInstance->CurrChapterNo = this->CurrChapterNo;
		SaveGameInstance->MaxChapterNo = this->MaxChapterNo;
		SaveGameInstance->CheckPointID = this->SavedCheckPointID;
		SaveGameInstance->bIsFirstLaunch = this->bIsFirstLaunch;
		SaveGameInstance->bEnableSmoothCharacterMovement = this->bEnableSmoothCharacterMovement;

		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
	}
}


void UISMGameInstance::LoadGame(UISMSaveGame* LoadedGame)
{
	CurrChapterNo = LoadedGame->CurrChapterNo;
	MaxChapterNo = LoadedGame->MaxChapterNo;
	SavedCheckPointID = LoadedGame->CheckPointID;
	bIsFirstLaunch = LoadedGame->bIsFirstLaunch;
	bEnableSmoothCharacterMovement = LoadedGame->bEnableSmoothCharacterMovement;
}

void UISMGameInstance::SetSelectedPawnClass(TSubclassOf<ACharacter> ServerCharacter, TSubclassOf<ACharacter> ClientCharacter)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// 서버에서만 실행
	if (World->GetNetMode() < ENetMode::NM_Client)
	{
		SelectedPawnClass = ServerCharacter;
		ClientPawnClass = ClientCharacter;
	}
}

void UISMGameInstance::SoundPlay(USoundBase* Sound)
{
	UGameplayStatics::SpawnSound2D(GetWorld(), Sound, 1, 1, 0, nullptr, true, true);
}

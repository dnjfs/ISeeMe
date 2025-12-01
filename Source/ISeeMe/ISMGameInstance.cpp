// Fill out your copyright notice in the Description page of Project Settings.

#include "ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UISMGameInstance::Init()
{
	Super::Init();

	// [NOTE] サーバーでスポーンしたアクターがクライアントにレプリケートされない問題への対処コード
	// 1. サーバーはスポーンするアクターのアーキタイプ情報を NetGUID を通じてクライアントに送信する。
	// 2. クライアントは受け取った NetGUID を使って、自身のメモリにロードされている UClass (アーキタイプ) を探す。
	// 3. SeamlessTravel 中に GC によってその UClass へのハードリファレンスがすべて消えると、
	//    UClass がメモリからアンロードされてアーキタイプを見つけられず、アクターのスポーンに失敗する。
	// 4. これを防ぐために、GameInstance で該当クラスを直接ロードしてハードリファレンスを保持する。
	//    こうすることで GC が UClass を破棄するのを防ぎ、ネットワーク経由でのキャラクタースポーンを常に保証する。
	//
	// - 参考: UClass は CDO を含んでおり、クライアントスポーン時には NetGUID で見つけた UClass の CDO を通じてスポーンされる。
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
		SaveGameInstance->AcquiredRecollectionIDs = this->AcquiredRecollectionIDs;
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
	AcquiredRecollectionIDs = LoadedGame->AcquiredRecollectionIDs;
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

	// サーバーのみで実行
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

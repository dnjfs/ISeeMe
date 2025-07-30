// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ISMCheckPoint.h"
#include "ISMCharacterState.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMCharacterState : public APlayerState
{
	GENERATED_BODY()

public:
	UStaticMeshComponent* GetRespawnPoint(int InCustomPlayerId);

	void SetCurCheckPoint(AISMCheckPoint* InCheckPoint);

public:
	/*First Check Point Location*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Spawn")
	FVector InitSpawnPointLocation;

	/*First Check Point Rotator*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Spawn")
	FRotator InitialSpawnPointRotator;

	/*Current Check Point*/
	UPROPERTY(Transient)
	TWeakObjectPtr<AISMCheckPoint> CurCheckPoint;

	/*Current Check Point State*/
	bool bCheckPoint = false;

	/*Player Number*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player Info")
	int32 CustomPlayerID;

	/* Switch Pawn After Select*/
	UFUNCTION(BlueprintCallable)
	void CallSelectPawn(TSubclassOf<APawn> NewPawn);

	UFUNCTION()
	void SelectPawn(TSubclassOf<APawn> NewPawn, int num);

	UFUNCTION(Server, Reliable)
	void ServerSelectPawn(TSubclassOf<APawn> NewPawn);

protected:
	virtual void BeginPlay();

};

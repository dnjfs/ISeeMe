// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISeeMeCharacter.h"
#include "ISMPlayerController.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class ISEEME_API AISMPlayerController : public APlayerController
{
	GENERATED_BODY()

	AISMPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	void SetOtherCharacter(AISeeMeCharacter* NewCharacter) { OtherCharacter = NewCharacter; }
	AISeeMeCharacter* GetOtherCharacter() { return OtherCharacter; }

	UFUNCTION()
	void OnRep_SwapCamera();

	UFUNCTION()
	void SwapCamera();

	UFUNCTION(Server, Reliable)
	void ServerCallSwapCamera();

	UFUNCTION(NetMulticast, Reliable)
	void RecoverAspect();

	UFUNCTION()
	void SwapAspect();

	UFUNCTION()
	void CurrentAspect();

	UPROPERTY(ReplicatedUsing = OnRep_SwapCamera)
	TObjectPtr<class AISeeMeCharacter> OtherCharacter;

private:
	bool bFirstAspect = false;
};

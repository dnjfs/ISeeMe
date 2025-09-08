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

protected:
	AISMPlayerController();

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void SetPawn(APawn* InPawn) override;

public:
	void SetOtherCharacter(AISeeMeCharacter* NewCharacter) { OtherCharacter = NewCharacter; }
	AISeeMeCharacter* GetOtherCharacter() { return OtherCharacter; }

	UFUNCTION()
	void OnRep_SwapCamera();

	/** Swap camera for Game Mode */
	UFUNCTION()
	virtual void SwapCamera();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Client call for Server swap camera*/
	UFUNCTION(Server, Reliable)
	void ServerCallSwapCamera();

	/** Swap aspect */
	UFUNCTION()
	void SwapAspect();

	/** Stay current aspect */
	UFUNCTION()
	void CurrentAspect();

	UPROPERTY(ReplicatedUsing = OnRep_SwapCamera)
	TObjectPtr<class AISeeMeCharacter> OtherCharacter;

	void DeadCharacter();

	UFUNCTION()
	void CallReadTutorial();

	UFUNCTION()
	void ReadTutorial();

	UFUNCTION(Server, Unreliable)
	void ServerReadTutorial();

	bool bDoneRead = false;

	UFUNCTION(Client, Reliable)
	void ClientGoToLevel();

	UFUNCTION(Client, Reliable)
	void ClientPlayLocalSound(USoundBase* SoundToPlay, bool bPlay);

private:
	bool bFirstAspect = false;

	/** Update Aspect */
	void UpdateAspect();

	/** Change First Aspect */
	void ChangeFirstAspect();

	/** Change Third Aspect */
	void ChangeThirdAspect();
};

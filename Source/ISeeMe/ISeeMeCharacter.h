// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OnlineSubsystemUtils.h"
#include "ISeeMeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UTimelineComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AISeeMeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AISeeMeCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** When first perspective, Camera Location */
	UPROPERTY(EditAnywhere, Category = "Bone")
	FVector FirstAspectLocation;

	/**  How high to dead */
	UPROPERTY(EditAnywhere)
	float DeadHeight=1000;

	/** Call Go Check Point Function from client to the server **/
	UFUNCTION(Server, Reliable)
	void ServerCallGoCheckPoint();

	/** Called for swap camera */
	void SwapCamera();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* CheckPointSound;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlaySound();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* GimmickAudioComponent;

	void EnableAudio();
	void DisableAudio();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetCameraRestore(bool bInIsRestored);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for focus input */
	void Focus();

	/** Called for swap aspect */
	void SwapAspect();

	/** Called for open menu */
	void OpenMenu();

	/** Called for go check point */
	void GoCheckPoint();

	/** Called for go check point */
	void CallGoCheckPoint();

	/** Called for control pitch */
	void ControlPitch(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Focus Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FocusAction;

	/** Swap Camera Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapCameraAction;

	/** Swap Aspect Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapAspectAction;

	/** Open Menu Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenMenuAction;

	/** Go Check Point Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GoCheckPointAction;

	/** Go Check Point Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ControlPitchAction;

	/* Focus */
	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* FocusTimeline;

	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FocusCurve;

	FRotator FocusStartRotator;
	FRotator FocusEndRotator;

	/*Judgement falling */
	bool bIsFalling = true;

	/* When fall, first location */
	float FirstHeight = 0;

	UFUNCTION()
	void PlayFocusTimeline(float Value);
};


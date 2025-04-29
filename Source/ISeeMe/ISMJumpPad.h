// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMJumpPad.generated.h"

UCLASS()
class ISEEME_API AISMJumpPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMJumpPad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* JumpPad;

	UPROPERTY(EditAnywhere)
	FVector LaunchVelocity;

	FVector OriginalLocation;

	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	class UTimelineComponent* ShakeTimeline;

	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ShakeCurve;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLaunchCharacter(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void PlayShakeTimeline(float Value);

	float ShakeScale = 50.f;
};

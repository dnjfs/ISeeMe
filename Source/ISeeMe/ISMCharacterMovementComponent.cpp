// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterMovementComponent.h"
#include "ISeeMeCharacter.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))


UISMCharacterMovementComponent::UISMCharacterMovementComponent(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 클라이언트가 서버에게서 받은 위치값으로 보정하는 시간
	NetworkSimulatedSmoothLocationTime = 0.4f; // 기본값: 0.1f

	// 서버 내 클라이언트의 pawn 위치를 보정하는 시간
	ListenServerNetworkSimulatedSmoothLocationTime = 0.3f; // 기본값: 0.04f
}
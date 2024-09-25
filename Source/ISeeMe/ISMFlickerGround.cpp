// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMFlickerGround.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AISMFlickerGround::AISMFlickerGround()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));

	RootComponent = GroundMesh;
}

// Called when the game starts or when spawned
void AISMFlickerGround::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
		SetActorTickEnabled(false);

	// 액터의 스태틱 메시 컴포넌트를 가져옵니다.
	UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>();
	if (Mesh && BaseMaterial)
	{
		// 머티리얼 인스턴스 다이나믹 생성
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

		// 메시에 머티리얼 인스턴스 적용
		Mesh->SetMaterial(0, DynamicMaterial);
	}

	ResetTimer();
}

void AISMFlickerGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority())
		return;

	if (bIsDormancy)
		return;

	RemainTime -= DeltaTime;

	// 남은시간 30% 남을 때부터 투명도 세팅
	if (RemainTime/FlickerInterval < 0.3f)
		MulticastSetTransparency((RemainTime / FlickerInterval) / 0.3f);

	if (RemainTime <= 0.f)
	{
		MulticastAwake(false);

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			this->ResetTimer();
		}, DormantTime, false);
	}
}

void AISMFlickerGround::ResetTimer()
{
	RemainTime = FlickerInterval;

	MulticastAwake(true);
	MulticastSetTransparency(RemainTime);
}

void AISMFlickerGround::MulticastAwake_Implementation(bool bInAwake)
{
	if (GroundMesh)
	{
		GroundMesh->SetVisibility(bInAwake);
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}

	bIsDormancy = !bInAwake;

	if (bInAwake)
		MulticastSetTransparency(1.f);
}

void AISMFlickerGround::MulticastSetTransparency_Implementation(float InOpacity)
{
	InOpacity = FMath::Clamp(InOpacity, 0.f, 1.f);

	// 머티리얼 인스턴스의 "Opacity"라는 이름의 파라미터를 설정
	if (DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), InOpacity);
}
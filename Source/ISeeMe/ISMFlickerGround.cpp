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

	// アクターのスタティックメッシュコンポーネントを取得する
	UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>();
	if (Mesh && BaseMaterial)
	{
		// マテリアルインスタンスダイナミックを作成
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

		// メッシュにマテリアルインスタンスを適用
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

	// 残り時間が 30% になったタイミングから透明度を設定
	if (RemainTime / FlickerInterval < 0.3f)
		MulticastSetTransparency((RemainTime / FlickerInterval) / 0.3f);

	if (RemainTime <= 0.f)
	{
		MulticastAwake(false);

		TWeakObjectPtr<AISMFlickerGround> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [WeakThis]()
			{
				if (TStrongObjectPtr<AISMFlickerGround> StrongThis = WeakThis.Pin(false))
				{
					StrongThis->ResetTimer();
				}
			}, DormantTime, false);
	}
}

void AISMFlickerGround::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
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

	// マテリアルインスタンスの「Opacity」という名前のパラメータを設定
	if (DynamicMaterial)
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), InOpacity);
}
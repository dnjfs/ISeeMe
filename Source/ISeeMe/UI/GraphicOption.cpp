// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/GraphicOption.h"
#include "UMG.h"
#include "Internationalization/StringTableRegistry.h"
#include "GameFramework/GameUserSettings.h"

void UGraphicOption::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (GEngine)
	{
		GameUserSettings = GEngine->GetGameUserSettings();
	}

	switch (GraphicType)
	{
	case EGraphicType::ANTIALIASING:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_AntiAliasing"));
		break;
	case EGraphicType::FOLIAGE:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_Foliage"));
		break;
	case EGraphicType::GLOBALILLUMINATION:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_GlobalIllumination"));
		break;
	case EGraphicType::POSTPROCESSING:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_PostProcessing"));
		break;
	case EGraphicType::REFLECTION:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_Reflection"));
		break;
	case EGraphicType::SHADING:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_Shading"));
		break;
	case EGraphicType::SHADOW:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_Shadow"));
		break;
	case EGraphicType::TEXTURE:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_Texture"));
		break;
	case EGraphicType::VIEWDISTANCE:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_ViewDistance"));
		break;
	case EGraphicType::VISUALEFFECT:
		OptionNameText->SetText(LOCTABLE("/Game/ISeeMe/Strings/ST_UI", "UI_VisualEffect"));
		break;
	}
}

void UGraphicOption::SetQuality(int32 Value)
{
	switch (GraphicType)
	{
	case EGraphicType::ANTIALIASING:
		GameUserSettings->SetAntiAliasingQuality(Value);
		break;
	case EGraphicType::FOLIAGE:
		GameUserSettings->SetFoliageQuality(Value);
		break;
	case EGraphicType::GLOBALILLUMINATION:
		GameUserSettings->SetGlobalIlluminationQuality(Value);
		break;
	case EGraphicType::POSTPROCESSING:
		GameUserSettings->SetPostProcessingQuality(Value);
		break;
	case EGraphicType::REFLECTION:
		GameUserSettings->SetReflectionQuality(Value);
		break;
	case EGraphicType::SHADING:
		GameUserSettings->SetShadingQuality(Value);
		break;
	case EGraphicType::SHADOW:
		GameUserSettings->SetShadowQuality(Value);
		break;
	case EGraphicType::TEXTURE:
		GameUserSettings->SetTextureQuality(Value);
		break;
	case EGraphicType::VIEWDISTANCE:
		GameUserSettings->SetViewDistanceQuality(Value);
		break;
	case EGraphicType::VISUALEFFECT:
		GameUserSettings->SetVisualEffectQuality(Value);
		break;
	}

	GameUserSettings->ApplySettings(false);
}

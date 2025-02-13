// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GearTextBlock.h"

#include "Internationalization/TextLocalizationManager.h"
#include "Internationalization/Culture.h"

#define CULTURE_EN "en"
#define CULTURE_FA "fa"

#define CULTURE_FA_VERTICAL_OFFSET_PER_FONT_SIZE -0.3f

UGearTextBlock::UGearTextBlock(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CultureChangeDelegateHandle = FInternationalization::Get().OnCultureChanged().AddUObject(this, &UGearTextBlock::OnCultureChanged);
}

void UGearTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	OnCultureChanged();
}

void UGearTextBlock::ReleaseSlateResources(bool bReleaseChildren)
{
	if (CultureChangeDelegateHandle.IsValid())
	{
		FInternationalization::Get().OnCultureChanged().Remove(CultureChangeDelegateHandle);
	}

	Super::ReleaseSlateResources(bReleaseChildren);
}

void UGearTextBlock::UpdateCultureVerticalOffset(const FString& Culture)
{
	float HeightOffset = 0.0f;

	if (Culture == CULTURE_FA)
	{
		if (bUseCustomHeightOffset)
		{
			HeightOffset = HeightOffset_Fa;
		}

		else
		{
			HeightOffset = CULTURE_FA_VERTICAL_OFFSET_PER_FONT_SIZE * (GetFont().HasValidFont() ? GetFont().Size : 10.0f);
		}
	}

	else if (Culture == CULTURE_EN)
	{
		if (bUseCustomHeightOffset)
		{
			HeightOffset = HeightOffset_En;
		}

		else
		{
			HeightOffset = 0.0f;
		}
	}

	SetRenderTranslation(FVector2D(0, HeightOffset));
}

void UGearTextBlock::OnCultureChanged()
{
	UpdateCultureVerticalOffset(FInternationalization::Get().GetCurrentCulture()->GetName());
}
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
	UpdateCultureVerticalOffset(FInternationalization::Get().GetCurrentCulture()->GetName());

	CultureChangeDelegateHandle = FInternationalization::Get().OnCultureChanged().AddUObject(this, &UGearTextBlock::OnCultureChanged);
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
	FVector2D RenderTranslation = FVector2D::ZeroVector;

	if (Culture == CULTURE_FA)
	{
		RenderTranslation = FVector2D(0, CULTURE_FA_VERTICAL_OFFSET_PER_FONT_SIZE * (GetFont().HasValidFont() ? GetFont().Size : 10.0f));
	}

	SetRenderTranslation(RenderTranslation);
}

void UGearTextBlock::OnCultureChanged()
{
	UpdateCultureVerticalOffset(FInternationalization::Get().GetCurrentCulture()->GetName());
}
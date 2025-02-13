// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "GearTextBlock.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UGearTextBlock : public UTextBlock
{
	GENERATED_UCLASS_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly)
	bool bUseCustomHeightOffset = false;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta=(EditCondition = bUseCustomHeightOffset))
	float HeightOffset_En = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, meta=(EditCondition = bUseCustomHeightOffset))
	float HeightOffset_Fa = 0.0f;

public:
	
protected:

	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	void UpdateCultureVerticalOffset(const FString& Culture);

	FDelegateHandle CultureChangeDelegateHandle;

	UFUNCTION()
	void OnCultureChanged();
};
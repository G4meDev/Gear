// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GearTypes.h"
#include "GearBaseWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAR_API UGearBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWidgetOrder WidgetOrder;


	UPROPERTY(BlueprintReadOnly)
	class AGearHUD* OwningHUD;
	
	friend class AGearHUD;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyPlayerPlatform.generated.h"

UCLASS()
class GEAR_API ALobbyPlayerPlatform : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* Root;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* RotationSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USceneComponent* DriverSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USkeletalMeshComponent* Vehicle; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UBoxComponent* Hitbox;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UWidgetComponent* PlayerbarWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<class AGearDriver> DriverClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PlatformIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationDrag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationDamping;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RotationStopBias;

	UPROPERTY()
	class AGearDriver* Driver;

	UPROPERTY()
	class ALobbyPlayerState* OwningPlayer;

	UPROPERTY()
	UMaterialInstanceDynamic* VehicleMID;

	bool bGrabbed;

	float RotationVelocity;

	friend class ALobbyGameState;

public:	
	ALobbyPlayerPlatform();
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	void ClearPlatform();

	UFUNCTION(BlueprintPure)
	class ALobbyPlayerState* GetOwningPlayer();

	UFUNCTION(BlueprintCallable)
	void Rotate(float Amount);

	UFUNCTION(BlueprintCallable)
	void ClearGrabbed();

	UFUNCTION(BlueprintCallable)
	void StopRotation();

	int32 GetPlatformIndex() const;

protected:

	UFUNCTION()
	void OnHitboxClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);

	UFUNCTION()
	void OnHitboxReleased(UPrimitiveComponent* TouchedComponent, FKey ButtonReleased);

	UFUNCTION()
	void OnHitboxTouchBegin(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	UFUNCTION()
	void OnHitboxTouchEnd(ETouchIndex::Type FingerIndex, UPrimitiveComponent* TouchedComponent);

	void SetOwningPlayer(class ALobbyPlayerState* InOwningPlayer);

	UFUNCTION()
	void PlayerCustomizationColorChanged();

	UFUNCTION()
	void PlayerCustomizationHeadChanged();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnOwningPlayerChanged();
};
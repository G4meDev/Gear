// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GearBuilderPawn.generated.h"

class AGearPlaceable;
class UPlaceableSocket;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionInstance;

UCLASS()
class GEAR_API AGearBuilderPawn : public APawn
{
	GENERATED_BODY()

public:
	
	AGearBuilderPawn();

	virtual void Tick(float DeltaTime) override;

	void StartPlacing();

	UFUNCTION(BlueprintCallable)
	void TeleportToRoadEnd();

	UFUNCTION(BlueprintCallable)
	void FlipSelectedRoadModule();

	void UpdateSelectedPlaceable();

	void SetSelectedPlaceable(AGearPlaceable* Placeable);
	bool HasSelectedPlaceable() const;

	UFUNCTION()
	void OnRep_SelectedPlaceable(AGearPlaceable* OldSelected);


protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveScreenInputTrigger(const FInputActionInstance& Instance);

	void MoveScreenInputCompleted(const FInputActionInstance& Instance);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* ViewTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UInputAction* MoveScreenAction;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MovementSpeed;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Damping;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Drag;

	FVector2D ScreenDragValue;
	FVector2D Velocity;

	bool bCanMove;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	UPlaceableSocket* SelectedSocket;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	bool bCanPlaceInSelectedSocket;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	bool bSelectedRoadModule;

	UPROPERTY(ReplicatedUsing=OnRep_SelectedPlaceable, BlueprintReadWrite)
	AGearPlaceable* SelectedPlaceable;

private:
	
	void MoveSelectedRoadModuleToEnd();

	bool bPlacingUnhandled;
};


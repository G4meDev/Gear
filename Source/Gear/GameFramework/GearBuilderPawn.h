// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GearBuilderPawn.generated.h"

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

protected:
	
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void FindStartPlacingTarget(FVector& Location, FRotator& Rotation);

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


public:	
	
	virtual void Tick(float DeltaTime) override;

	void StartPlacing();
};

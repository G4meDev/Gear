// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GearBuilderPawn.generated.h"

class AGearPlaceable;
class AGearRoadModule;
class UPlaceableSocket;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionInstance;

UENUM(BlueprintType)
enum class EBuilderPawnState : uint8
{
	Preview,
	PlacingRoadModules,
	PlacingHazards,
	Waiting
};

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
	void PlaceRoadModule();

	void SetSelectedPlaceable(AGearPlaceable* Placeable);
	bool HasSelectedPlaceable() const;

	UFUNCTION()
	void OnRep_SelectedPlaceable(AGearPlaceable* OldSelected);

	UFUNCTION()
	void OnRep_SelectedPlaceableClass();

	void OnRoadModuleSocketChanged();

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bPlacedModule;

protected:
	
	virtual void BeginPlay() override;

	virtual void NotifyControllerChanged() override;

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
	EBuilderPawnState BuilderPawnState;

	// in preview
	UPROPERTY(ReplicatedUsing=OnRep_SelectedPlaceable, BlueprintReadWrite)
	AGearPlaceable* SelectedPlaceable;

	UPROPERTY(ReplicatedUsing=OnRep_SelectedPlaceableClass, BlueprintReadWrite)
	TSubclassOf<AGearPlaceable> SelectedPlaceableClass;

	UPROPERTY()
	AGearRoadModule* PlacingRoadModule;
	UPROPERTY()
	AGearRoadModule* PlacingRoadModule_MirroredX;
	UPROPERTY()
	AGearRoadModule* PlacingRoadModule_MirroredY;
	UPROPERTY()
	AGearRoadModule* PlacingRoadModule_MirroredX_MirroredY;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	bool bSelectedMirroredX;
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly)
	bool bSelectedMirroredY;

	AGearRoadModule* SpawnRoadModuleLocally(TSubclassOf<AGearPlaceable> SpawnClass);

	void SpawnPlacingRoadModules();

	AGearRoadModule* GetActiveRoadModule() const;

	UFUNCTION(BlueprintCallable)
	void UpdatePlacingRoadModule(bool bMirroredX, bool bMirroredY);

	void Cleanup_SpawnedActors();

	friend class AGearGameMode;

private:

};


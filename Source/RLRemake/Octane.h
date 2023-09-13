// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Octane.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RLREMAKE_API AOctane : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOctane();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteerAction;

private:
	void Throttle(const FInputActionValue& Value);
	void Steer(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = Root)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, Category = Mesh)
	UStaticMeshComponent* OctaneMesh;

	UPROPERTY(EditAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = CarSettings)
	float MaxThrottleSpeed{};

	UPROPERTY(EditAnywhere, Category = CarSettings)
	float ThrottleAcceleration{};

	UPROPERTY(EditAnywhere, Category = CarSettings)
	float TurnAngle{};

	float CurrentTurnAngle{};

	float ThrottleValue{};
	bool bIsGrounded{};
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Octane.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AOctane::AOctane()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the mesh component
	OctaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	OctaneMesh->SetSimulatePhysics(true);
	RootComponent = OctaneMesh;

	// Create the spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(OctaneMesh);

	// Create the camera component and attach it to the spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AOctane::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AOctane::Throttle(const FInputActionValue& Value)
{
	ThrottleValue = Value.Get<float>();
}

void AOctane::Steer(const FInputActionValue& Value)
{
	CurrentTurnAngle = Value.Get<FVector2D>().X * TurnAngle;
}

void AOctane::Jump(const FInputActionValue& Value)
{
	if (bIsGrounded)
	{
		FVector JumpForceVector = OctaneMesh->GetUpVector() * JumpForce;
		OctaneMesh->AddForce(JumpForceVector);
	}
}

//only call this function once per frame and just save the value of it in bIsGrounded
bool AOctane::IsGrounded()
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 10.0f); // Adjust the Z value for your needs

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the calling pawn itself

	// Perform the Line Trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	// Debug Visualization
	if (bHit)
	{
		// If the Line Trace hit something, draw a green line from StartLocation to the hit point
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 1);
	}
	else
	{
		// If the Line Trace didn't hit anything, draw a red line to the EndLocation
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f, 0, 1);
	}

	// Return the result of the Line Trace
	return bHit;
}

// Called every frame
void AOctane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsGrounded = IsGrounded();

	if (bIsGrounded) // the car is on the ground
	{
		//if throttling
		if (ThrottleValue > 0.1f)
		{
			//if steering, rotate first before moving
			if (CurrentTurnAngle)
			{
				// Get the current relative rotation of the OctaneMesh
				FRotator CurrentRelativeRotation = OctaneMesh->GetRelativeRotation();

				// Add the rotation value of 10 degrees around the Z-axis
				FRotator IncrementalRotation(0.f, CurrentTurnAngle * DeltaTime, 0.f);
				FRotator NewRelativeRotation = CurrentRelativeRotation + IncrementalRotation;

				// Set the new relative rotation for the OctaneMesh
				OctaneMesh->SetRelativeRotation(NewRelativeRotation);
			}

			UE_LOG(LogTemp, Warning, TEXT("The linear velocity is: %f"), OctaneMesh->GetPhysicsLinearVelocity().Length())

			// Calculate forward movement based on input
			FVector ForwardForce = OctaneMesh->GetForwardVector() * ThrottleValue * ThrottleAcceleration;

			// Apply the forward force to the car's physics body
			OctaneMesh->AddForce(ForwardForce);

			// If the car is too fast, cap the speed from throttling
			FVector velocity = OctaneMesh->GetPhysicsLinearVelocity();
			float speed = velocity.Length();
			if (speed > MaxThrottleSpeed)
			{
				OctaneMesh->SetPhysicsLinearVelocity(velocity / speed * MaxThrottleSpeed);
			}
		}
	}
	else // The car is in the air
	{
		//if steering, rotate first before moving
		if (CurrentTurnAngle)
		{
			// Get the current relative rotation of the OctaneMesh
			FRotator CurrentRelativeRotation = OctaneMesh->GetRelativeRotation();

			// Add the rotation value of 10 degrees around the Z-axis
			FRotator IncrementalRotation(0.f, CurrentTurnAngle * DeltaTime, 0.f);
			FRotator NewRelativeRotation = CurrentRelativeRotation + IncrementalRotation;

			// Set the new relative rotation for the OctaneMesh
			OctaneMesh->SetRelativeRotation(NewRelativeRotation);
		}
	}


	// reset values for input
	ThrottleValue = 0.f;
	CurrentTurnAngle = 0.f;
}

// Called to bind functionality to input
void AOctane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AOctane::Throttle);
		EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Triggered, this, &AOctane::Steer);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOctane::Jump);
	}

}


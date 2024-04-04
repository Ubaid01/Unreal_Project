// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalmeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABird::ABird()
{

	PrimaryActorTick.bCanEverTick = true;
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);
	Capsule -> SetCapsuleHalfHeight(22.0f);
	Capsule -> SetCapsuleRadius(15.0f);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh -> SetupAttachment(GetRootComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArm -> SetupAttachment(GetRootComponent());
	SpringArm -> TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera -> SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput :: Player0 ;

}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	// OR Simply Controller ( as its in public section in UE 5.1) ( Also can declare directly )
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()) )
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer :: GetSubsystem<UEnhancedInputLocalPlayerSubsystem>( PlayerController -> GetLocalPlayer());
		if (Subsystem) 
		{
			Subsystem -> AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::Move(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>(); // Has to be same type as IA_Move
	if ((GetController() != nullptr) && (DirectionValue != 0.0f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue);
	}

	//const FVector2D MyVector = Value.Get<FVector2D>();
	//if ( ( GetController() ) && ( ( MyVector.X != 0.0f ) || ( MyVector.Y != 0.0f ) ) ) {
	//	FVector Forward = GetActorForwardVector();
	//	AddMovementInput(Forward, MyVector.X);
	//	FVector Right = GetActorRightVector();
	//	AddMovementInput(Right, MyVector.Y);

	//}
}

void ABird::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if( GetController() ) 
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent -> BindAction(MoveAction, ETriggerEvent :: Triggered, this, &ABird :: Move);
		EnhancedInputComponent -> BindAction(LookAction, ETriggerEvent :: Triggered, this, &ABird :: Look);
	}

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "GroomComponent.h"


ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement() -> bOrientRotationToMovement = true ;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 400.0f, 0.0f);


	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom -> SetupAttachment(GetRootComponent());
	CameraBoom -> TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera -> SetupAttachment(CameraBoom);

	//Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	//Hair -> SetupAttachment(GetMesh());
	//Hair -> AttachmentName = FString("Head");

	//Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	//Eyebrows -> SetupAttachment(GetMesh());
	//Eyebrows -> AttachmentName = FString("Head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASlashCharacter::MoveForward(float Value)
{
	if ((GetController()) && (Value != 0.0f)) 
	{
		// Find out which way is Controller Forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f); // As character can only move sideways not updown so we only need to get the X-axis( Unreal's Y-Axis ) Rotation
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis( EAxis :: X ); // This will return the single Unit Vector for direction corresponding to X-axis( Forawrd , Backward ) with Yaw Rotation.
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveSideways(float Value)
{
	if ((GetController()) && (Value != 0.0f)) 
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f); 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis :: Y); // Same here Get the Updated X-Axis( Yaw Rotation ) But corresponding to Right , Left ( Y-Axis in Unreal )
		AddMovementInput(Direction, Value);

	}
}

void ASlashCharacter::Turn(float Value)
{
	if( ( GetController() ) && ( Value != 0.0f ) ) 
	{
		AddControllerYawInput(Value);
	}
}

void ASlashCharacter::LookUp(float Value)
{
	if ((GetController()) && (Value != 0.0f))
	{
		AddControllerPitchInput(Value);
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent -> BindAxis(FName("MoveForward"), this, &ASlashCharacter :: MoveForward);
	PlayerInputComponent -> BindAxis(FName("MoveSideways"), this, &ASlashCharacter :: MoveSideways);
	PlayerInputComponent -> BindAxis(FName("Turn"), this, &ASlashCharacter :: Turn);
	PlayerInputComponent -> BindAxis(FName("LookUp"), this, &ASlashCharacter :: LookUp);
	PlayerInputComponent -> BindAction(FName("Jump"), IE_Pressed , this, &ACharacter :: Jump);


}


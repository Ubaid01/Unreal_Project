// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance :: NativeInitializeAnimation() // Similiar to Animation BeginePlay()
{
	Super :: NativeInitializeAnimation() ;

	SlashCharacter = Cast<ASlashCharacter>( TryGetPawnOwner() );
	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter -> GetCharacterMovement();
	}
}

void USlashAnimInstance :: NativeUpdateAnimation(float DeltaTime) 
{
	Super :: NativeUpdateAnimation(DeltaTime);
	
	if( SlashCharacterMovement ) 
	{
		GroundSpeed = UKismetMathLibrary :: VSizeXY(SlashCharacterMovement -> Velocity );
		Velocity = SlashCharacter -> GetVelocity();
		IsFalling = SlashCharacterMovement -> IsFalling();
		CharacterState = SlashCharacter -> GetCharacterState();
	}
}
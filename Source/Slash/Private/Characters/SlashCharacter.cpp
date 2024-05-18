// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"


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

}

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if ( EquippedWeapon && EquippedWeapon -> GetWeaponBox( ) ) 
	{
		EquippedWeapon -> GetWeaponBox() -> SetCollisionEnabled(CollisionEnabled); 
		EquippedWeapon -> IgnoreActors.Empty(); // Empty the IgnoreActors for each time the notify is ON-OFF
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add("SlashCharacter") ;
	
}

void ASlashCharacter::MoveForward(float Value)
{
	if ((GetController()) && (Value != 0.0f)) 
	{
		if (ActionState != EActionState::EAS_Unoccupied) return ;

		// Find out which way is Controller Forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f); // As character can only move sideways not updown so we only need to get the X-axis( Unreal's Y-Axis ) Rotation
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis( EAxis :: X ); // This will return the single Unit Vector for direction corresponding to X-axis( Forawrd , Backward ) with Yaw Rotation.
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveSideways(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;

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

bool ASlashCharacter::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied 
		&& CharacterState != ECharacterState::ECS_Unequipped ;
}

bool ASlashCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon ;
}

void ASlashCharacter::Disarm()
{
	if (EquippedWeapon) 
	{
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("RightHandSocket") );
	}
}

void ASlashCharacter::FinsihEquipping()
{
	ActionState = EActionState::EAS_Unoccupied ; 
}

void ASlashCharacter::EquipAction()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>( GetOverlappingItem() );
	if (OverlappingWeapon) 
	{
		OverlappingWeapon -> Equip( GetMesh(), FName("RightHandSocket") , this , this );
		CharacterState = ECharacterState :: ECS_EquippedOneHandWeapon ;
		OverlappingItem = nullptr ; // After Attatching Item to mesh set that Item to Null.
		EquippedWeapon = OverlappingWeapon ;
	}
	else 
	{
		// So that it also no runs in between the attack animation
		if ( CanDisarm( ) ) 
		{
			PlayEquipMontage(FName("UnEquip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon ;
		}
		else if ( CanArm( ) ) 
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
			ActionState = EActionState::EAS_EquippingWeapon ;
		}
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{

	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && EquipMontage) 
	{
		AnimInstance -> Montage_Play(EquipMontage);
		AnimInstance -> Montage_JumpToSection(SectionName, EquipMontage);
	}

}

bool ASlashCharacter::CanAttack() const
{
	return 	ActionState == EActionState :: EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Attack()
{
	if ( CanAttack( ) )
	{
		PlayAttackMontage() ;
		ActionState = EActionState :: EAS_Attacking ;

	}
}

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && AttackMontage) // Check If AttackMontage is also set from Details in Blueprint OR Not
	{
		AnimInstance -> Montage_Play(AttackMontage);
		const int32 Selection = FMath :: RandRange(0, 2); // Is used to play different Montages Slots at random
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		default:
			break; // Will give error even if last break NOT given
		}
		AnimInstance -> Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied ;
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
	PlayerInputComponent -> BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter :: EquipAction );
	PlayerInputComponent -> BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter :: Attack);

}


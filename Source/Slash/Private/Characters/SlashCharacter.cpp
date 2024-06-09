// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true ;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement() -> bOrientRotationToMovement = true ;
	GetCharacterMovement() -> RotationRate = FRotator(0.0f, 400.0f, 0.0f);

	GetMesh() -> SetCollisionObjectType(ECollisionChannel :: ECC_WorldDynamic) ;
	GetMesh() -> SetCollisionResponseToAllChannels(ECollisionResponse :: ECR_Ignore);
	GetMesh() -> SetCollisionResponseToChannel( ECollisionChannel :: ECC_Visibility , ECollisionResponse :: ECR_Block ) ;
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_WorldDynamic, ECollisionResponse :: ECR_Overlap);
	GetMesh() -> SetGenerateOverlapEvents( true ) ; 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom -> SetupAttachment(GetRootComponent());
	CameraBoom -> TargetArmLength = 300.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera -> SetupAttachment(CameraBoom);

}

void ASlashCharacter::Tick(float DeltaTime)
{
	if ( Attributes && ( Attributes -> GetStaminaPercent() < 1.0f && Attributes -> GetStaminaPercent() >= 0.0f ) )
	{
		Attributes -> RegenerateStamina(DeltaTime) ;
		SlashOverlay -> SetStaminaBarPercent( Attributes -> GetStaminaPercent() ) ;
	}
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes)
	{
		Attributes -> AddToSouls( Soul -> GetSouls() ) ;
		SlashOverlay -> SetSouls( Attributes -> GetSouls() ) ;
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if ( Attributes && SlashOverlay )
	{
		Attributes -> AddToGold( Treasure -> GetGold() );
		SlashOverlay -> SetGold( Attributes -> GetGold() );
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add("EngageableTarget") ;

	InitializeSlashOverlay();
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent -> BindAxis(FName("MoveForward"), this, &ASlashCharacter :: MoveForward);
	PlayerInputComponent -> BindAxis(FName("MoveSideways"), this, &ASlashCharacter :: MoveSideways);
	PlayerInputComponent -> BindAxis(FName("Turn"), this, &ASlashCharacter :: Turn);
	PlayerInputComponent -> BindAxis(FName("LookUp"), this, &ASlashCharacter :: LookUp);
	PlayerInputComponent -> BindAction(FName("Jump"), IE_Pressed , this, &ASlashCharacter :: Jump);
	PlayerInputComponent -> BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter :: EquipAction );
	PlayerInputComponent -> BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter :: Attack);
	PlayerInputComponent -> BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter :: Dodge );

}

void ASlashCharacter::Jump()
{
	if ( IsUnoccupied() || IsAttributeAlive() )
		Super::Jump() ;
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage( DamageAmount ) ;
	SetHUDHealth();
	return DamageAmount ;
}

void ASlashCharacter::GetHit_Implementation( const FVector& ImpactPoint, AActor* Attacker )
{
	Super :: GetHit_Implementation( ImpactPoint , Attacker ) ;
	SetWeaponCollisionEnabled( ECollisionEnabled :: NoCollision ) ;

	if ( Attributes && Attributes -> GetHealthPercent() > 0 ) // Since after each thing our character was getting hit react.
		ActionState = EActionState::EAS_HitReaction ;
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

void ASlashCharacter::EquipAction()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>( GetOverlappingItem() ) ;
	if (OverlappingWeapon) 
	{
		if ( EquippedWeapon )
		{
			EquippedWeapon -> DropWeapon( ) ;
			CharacterState = ECharacterState::ECS_Unequipped ;
		}
		EquipWeapon( OverlappingWeapon ) ;
	} 
	else 
	{
		// So that it also no runs in between the attack animation
		if ( CanDisarm( ) ) 
		{
			Disarm( ) ;
		}
		else if ( CanArm( ) ) 
		{
			Arm( ) ;
		}
	}
}

void ASlashCharacter :: EquipWeapon(AWeapon* Weapon) 
{
	Weapon -> Equip(GetMesh(), FName("RightHandSocket"), this, this) ;
	CharacterState = ECharacterState :: ECS_EquippedOneHandWeapon ;
	OverlappingItem = nullptr; // After Attatching Item to mesh set that Item to Null.
	EquippedWeapon = Weapon;
}
void ASlashCharacter::Attack()
{
	Super ::Attack() ;
	if ( CanAttack( ) )
	{
		PlayAttackMontage() ;
		ActionState = EActionState :: EAS_Attacking ;

	}
}

void ASlashCharacter::Dodge()
{
	const bool CanNotDodge = ( ( ActionState == EActionState::EAS_HitReaction) || (ActionState == EActionState::EAS_Attacking) || ( ! HasEnoughStamina( ) ) ) ;
	if ( CanNotDodge ) return ; 

	ActionState = EActionState :: EAS_Dodging ;
	PlayDodgeMontage( ) ;
	if ( SlashOverlay ) // As checked Attributes in HasEnoughStamina so no need to do again.
	{
		Attributes -> UseStamina( Attributes -> GetDodgeCost() ) ;
		SlashOverlay -> SetStaminaBarPercent( Attributes -> GetStaminaPercent() ) ;
	}
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && ( Attributes -> GetStamina() > Attributes -> GetDodgeCost() ) ;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied ;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd() ;
	ActionState = EActionState::EAS_Unoccupied ;
}

bool ASlashCharacter::CanAttack()
{
	return 	ActionState == EActionState :: EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied
		&& CharacterState == ECharacterState::ECS_Unequipped
		&& EquippedWeapon ;
}

void ASlashCharacter::Arm() 
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool ASlashCharacter::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied 
		&& CharacterState != ECharacterState::ECS_Unequipped ;
}

void ASlashCharacter::Disarm() 
{
	PlayEquipMontage(FName("UnEquip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("RightHandSocket") );
	}
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon) 
	{
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied ;
}

void ASlashCharacter::FinsihEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
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

void ASlashCharacter::Die()
{
	ActionState = EActionState::EAS_Dead ;
	Super :: Die() ;
	DisbaleMeshCollision() ;
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController -> GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD -> GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay -> SetHealthBarPercent( Attributes ->  GetHealthPercent() ) ;
				SlashOverlay -> SetStaminaBarPercent(1.0f); // 100.0 %
				SlashOverlay -> SetGold(0);
				SlashOverlay -> SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay -> SetHealthBarPercent(Attributes -> GetHealthPercent());
	}
}
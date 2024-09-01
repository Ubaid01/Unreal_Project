// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Potion.h"

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
	if ( Attributes && SlashOverlay && ( ActionState != EActionState::EAS_Dead ) )
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

void ASlashCharacter::AddHealth(APotion* Potion)
{
	if ( Attributes && SlashOverlay ) 
	{
		int32 GoldCoins = Attributes -> GetGold();
		int32 Souls = Attributes -> GetSouls();
		float HealthPoitns = Attributes -> GetHealthPercent();
		int32 GoldCost = Potion -> GetPotionGoldCost();
		int32 SoulsCost = Potion -> GetPotionSoulsCost();

		if ( Attributes -> GetHealthPercent() >= 0.97f )
		{
			if ( GEngine )
			{
				GEngine -> AddOnScreenDebugMessage(3, 5.0f, FColor::Cyan, FString("Health is Full!") );
				return ;
			}
		}

		if ( GoldCoins >= GoldCost ) 
		{
			Attributes -> AddToGold( - GoldCost ) ;
			SlashOverlay -> SetGold( Attributes -> GetGold() ) ;
			SlashOverlay -> SetHealthBarPercent( FMath :: Clamp( Attributes -> GetHealthPercent() + Potion -> GetPoints() / 100.0f, 0.0f, Attributes -> GetMaxHealth() ) ) ;
			PlayPotionMontage();
			Potion -> Destroy() ;
		}
		else if ( Souls >= SoulsCost )
		{
			Attributes -> AddToSouls( -SoulsCost );
			SlashOverlay -> SetSouls( Attributes -> GetSouls() );
			SlashOverlay -> SetHealthBarPercent( FMath :: Clamp( Attributes -> GetHealthPercent() + Potion -> GetPoints() / 100.0f, 0.0f, Attributes -> GetMaxHealth() ) ) ;
			PlayPotionMontage();
			Potion -> Destroy();
		}
		else 
		{
			if ( GEngine )
			{
				FString DebugMessage = FString::Printf(TEXT("Max Potion Cost: Gold = %d OR Souls = %d"), GoldCost, SoulsCost);
				GEngine -> AddOnScreenDebugMessage(3, 5.0f, FColor::Red, DebugMessage ) ;
			}
		}

	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>( Controller ) )
	{
		if ( UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer :: GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController -> GetLocalPlayer() ) )
		{
			Subsystem -> AddMappingContext(SlashContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget")) ;
	InitializeSlashOverlay();
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent -> BindAction(MovementAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: Move);
		EnhancedInputComponent -> BindAction(LookingAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: Look);
		EnhancedInputComponent -> BindAction(JumpAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: Jump);
		EnhancedInputComponent -> BindAction(AttackAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: Attack);
		EnhancedInputComponent -> BindAction(EquippingAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: EquipAction);
		EnhancedInputComponent -> BindAction(DodgeAction, ETriggerEvent :: Triggered, this, &ASlashCharacter :: Dodge);
	}

	//PlayerInputComponent -> BindAxis(FName("MoveForward"), this, &ASlashCharacter :: MoveForward);
	//PlayerInputComponent -> BindAction(FName("Jump"), IE_Pressed , this, &ASlashCharacter :: Jump);
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

	if ( Attributes && Attributes -> GetHealthPercent() > 0.0f ) // Since after each thing our character was getting hit react.
		ActionState = EActionState::EAS_HitReaction ;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>() ;
	if ( ! Controller || MovementVector.IsZero() || ActionState != EActionState::EAS_Unoccupied ) return ;

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation( 0.0f, Rotation.Yaw, 0.0f ) ; 
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis :: X);
	AddMovementInput( ForwardDirection, MovementVector.Y ) ;

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis :: Y);
	AddMovementInput( RightDirection, MovementVector.X ) ; // Scale by 'X' as 'X' vector will be filled with value.
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	if ( Controller && ( ! LookAxisVector.IsZero( )  ) ) // IsZero() checked to prevent unecessary callbacks.
	{
		AddControllerYawInput( LookAxisVector.X );
		AddControllerPitchInput( LookAxisVector.Y );
	}
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
	const bool CanDodge = ( ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_EquippingWeapon ) && ( ActionState != EActionState::EAS_Dodging ) && HasEnoughStamina() ; // This also allows the character to dodge while running.
	if ( !CanDodge ) return ;

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

void ASlashCharacter::PlayPotionMontage()
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && HealthPotionMontage)
	{
		if ( HealhPotionSections.Num() <= 0 ) return ;
		const int32 MaxI = HealhPotionSections.Num() - 1;
		const int32 i = FMath :: RandRange( 0, MaxI );
		AnimInstance -> Montage_Play(HealthPotionMontage);
		AnimInstance -> Montage_JumpToSection( HealhPotionSections[i], EquipMontage);
	}
}

void ASlashCharacter::Die_Implementation()
{
	ActionState = EActionState::EAS_Dead ;
	Super :: Die_Implementation() ;
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
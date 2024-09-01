// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "InputActionValue.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class ASoul ;
class ATreasure ;
class UAnimMontage;
class USlashOverlay ;
class UInputMappingContext;
class UInputAction;
class APotion;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick( float DeltaTime ) override ;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override ;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint , AActor* Attacker ) override;

	virtual void SetOverlappingItem(AItem* Item) override { OverlappingItem = Item; }
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState;  }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	virtual void AddSouls( ASoul* Soul ) override ;
	virtual void AddGold( ATreasure* Treasure ) override ;
	virtual void AddHealth( APotion* Potion ) override;
protected:
	virtual void BeginPlay() override;
	/* Enhanced Input Actions */
	UPROPERTY( EditAnywhere , Category="Input" )
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookingAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquippingAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	/*  Callbacks for Input  */
	void MoveForward(float Value);
	void MoveSideways(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipAction();
	void EquipWeapon(AWeapon* Weapon) ;
	virtual void Attack() override ;
	void Dodge() ;
	bool HasEnoughStamina();

	/* Combat */
	virtual void AttackEnd() override ;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override ;
	bool CanArm() const;
	void Arm() ;
	bool CanDisarm() const;
	void Disarm() ;
	UFUNCTION( BlueprintCallable )
	void AttachWeaponToHand();

	UFUNCTION( BlueprintCallable )
	void AttachWeaponToBack();

	UFUNCTION( BlueprintCallable )
	void HitReactEnd( ) ;

	UFUNCTION(BlueprintCallable)
	void FinsihEquipping( ) ;
	void PlayEquipMontage(const FName& SectionName ) ;
	void PlayPotionMontage() ;
	virtual void Die_Implementation() override ;

private:
	bool IsUnoccupied();
	void InitializeSlashOverlay();
	void SetHUDHealth();
	/* Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY( VisibleInstanceOnly ) 
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HealthPotionMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TArray<FName> HealhPotionSections;

	UPROPERTY()
	USlashOverlay* SlashOverlay ;

	ECharacterState CharacterState = ECharacterState :: ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState :: EAS_Unoccupied;

};

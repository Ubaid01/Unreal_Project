// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "BaseCharacter.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint , AActor* Attacker ) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item ; }
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState;  }

protected:
	virtual void BeginPlay() override;
	/*  Callbacks for Input  */
	void MoveForward(float Value);
	void MoveSideways(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipAction();
	void EquipWeapon(AWeapon* Weapon) ;
	virtual void Attack() override ;

	/* Combat */
	virtual void AttackEnd() override ;
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

private:

	/* Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY( VisibleInstanceOnly ) 
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState :: ECS_Unequipped;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState :: EAS_Unoccupied;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;


UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item ; }
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState;  }

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled :: Type CollisionEnabled); 

protected:
	virtual void BeginPlay() override;
	/* 
	Callbacks for Input 
	*/
	void MoveForward(float Value);
	void MoveSideways(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipAction();
	void Attack();

	/* 
	Play Montage Functions 
	*/
	void PlayAttackMontage();
	UFUNCTION( BlueprintCallable )
	void AttackEnd();
	bool CanAttack() const ;
	void PlayEquipMontage( FName SectionName ) ;
	bool CanDisarm() const;
	bool CanArm() const;

	UFUNCTION( BlueprintCallable )
	void Disarm();

	UFUNCTION( BlueprintCallable )
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinsihEquipping() ;

private:

	ECharacterState CharacterState = ECharacterState :: ECS_Unequipped ; 
	UPROPERTY( BlueprintReadWrite , meta = ( AllowPrivateAccess = "true") ) 
	EActionState ActionState = EActionState :: EAS_Unoccupied ;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY( VisibleInstanceOnly ) 
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") // UPROPERTY so it particicaptes in garbage collection
	AWeapon* EquippedWeapon ;

	/* 
		Animation Montages
	*/

	UPROPERTY( EditDefaultsOnly , Category = "Montages" )
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

};

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

	/* 
		Animation Montages
	*/

	UPROPERTY( EditDefaultsOnly , Category = "Montages" )
	UAnimMontage* AttackMontage;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;


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
	void MoveForward(float Value);
	void MoveSideways(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipAction();

private:

	ECharacterState CharacterState = ECharacterState :: ECS_Unequipped ; 

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY( VisibleInstanceOnly ) 
	AItem* OverlappingItem;


};

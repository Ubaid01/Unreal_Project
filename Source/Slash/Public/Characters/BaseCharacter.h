// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class AWeapon ;
class UAttributeComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter , public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled :: Type CollisionEnabled);


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") // UPROPERTY so it particicaptes in garbage collection
	AWeapon* EquippedWeapon;

	virtual void Attack() ; // Made virtual so to implement in Child class.
	virtual void PlayAttackMontage() ;

	UFUNCTION(BlueprintCallable) // Since inheriting so only need UFUNCTION 1 time.
	virtual void AttackEnd();
	virtual bool CanAttack() const ;
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitReactMonatge(const FName& SectionName); // Also used const referenced here to avoid copying
	virtual void PlayDeathMontage() ; // Die for Stephen

	/* 
	Animation Montages 
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	/*
	Components
	*/

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

};

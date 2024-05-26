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

	virtual void Attack() ;
	virtual void PlayAttackMontage() ;

	UFUNCTION(BlueprintCallable) 
	virtual void AttackEnd();
	virtual bool CanAttack() ;
	virtual bool IsAttributeAlive();

	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitReactMonatge(const FName& SectionName); 
	virtual void PlayDeathMontage() ; // Die for Stephen
	void PlayHitSound(const FVector& ImpactPoint) ;
	void SpawnHitParticles( const FVector& ImpactPoint ) ;
	virtual void HandleDamage( float DamageAmount ) ;

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

private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UParticleSystem* HitParticles;

};

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

protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation( const FVector& ImpactPoint , AActor* Attacker ) override ;

	/* Combat */
	virtual void Attack() ;
	virtual void Die() ;
	virtual bool CanAttack();
	virtual void HandleDamage(float DamageAmount);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled :: Type CollisionEnabled);
	void DisableCapsule() ;
	/* Combat */

	/* <Animation Montages */
	virtual int32 PlayAttackMontage();
	void PlayHitReactMonatge(const FName& SectionName);
	virtual int32 PlayDeathMontage();
	/* <Animation Montages */

	virtual bool IsAttributeAlive();
	UPROPERTY(VisibleAnywhere, Category = "Weapon") // UPROPERTY so it particicaptes in garbage collection
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

private:
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontage(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;
	/*
		Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> DeathMontageSections;

};

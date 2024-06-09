// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
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
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }

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
	virtual void DodgeEnd();
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled :: Type CollisionEnabled);
	void DisableCapsule() ;
	/* Combat */

	/* <Animation Montages */
	virtual int32 PlayAttackMontage();
	void PlayHitReactMonatge(const FName& SectionName);
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage() ;
	void StopAttackMontage() ;

	/* <Animation Montages */
	UFUNCTION( BlueprintCallable )
	FVector GetTranslationWarpTarget() ;
	UFUNCTION( BlueprintCallable )
	FVector GetRotationWarpTarget();
	virtual bool IsAttributeAlive();
	void DisbaleMeshCollision() ;

	UPROPERTY(VisibleAnywhere, Category = "Weapon") // UPROPERTY so it particicaptes in garbage collection
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;
	UPROPERTY(BlueprintReadOnly, Category = "Combat") // For Motion Warping
	AActor* CombatTarget;

	UPROPERTY( EditAnywhere , Category = "Combat" )
	double WarpTargetDistance = 75.0f ;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

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
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TArray<FName> DodgeMontageSections;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ACharacter , public IHitInterface 
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser ) override ;

protected:
	virtual void BeginPlay() override;

	/* 
		Montage Play Functions
	*/
	void PlayHitReactMonatge(const FName& SectionName); // Also used const referenced here to avoid copying
	void PlayDeathMontage() ;

	UPROPERTY(BlueprintReadOnly) 
	EDeathPose DeathPose = EDeathPose :: EDP_Alive;

private:
	UPROPERTY( VisibleAnywhere )
	UAttributeComponent* Attributes;

	UPROPERTY( VisibleAnywhere )
	UHealthBarComponent* HealthBarWidget;

	/* 
		Animation Montages 
	*/
	UPROPERTY( EditDefaultsOnly , Category = "Montages" )
	UAnimMontage* HitReactMontage; // Don't forget to setup it in Blueprint also

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY( EditAnywhere , Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY( EditAnywhere , Category = "Visual Effects")
	UParticleSystem* HitParticles ;

	UPROPERTY( ) // To make sure its initialize to nullptr not garbage.
	AActor* CombatTarget ;

	UPROPERTY( EditAnywhere ) 
	double CombatRadius = 500.0f ;
};

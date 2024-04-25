// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UAnimMontage;

UCLASS()
class SLASH_API AEnemy : public ACharacter , public IHitInterface 
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);

protected:
	virtual void BeginPlay() override;
	/* 
		Montage Play Functions
	*/
	void PlayHitReactMonatge(const FName& SectionName); // Also used const referenced here to avoid copying

private:
	/* 
		Animation Montages 
	*/
	UPROPERTY( EditDefaultsOnly , Category = "Montages" )
	UAnimMontage* HitReactMontage; // Don't forget to setup it in Blueprint also

	UPROPERTY( EditAnywhere , Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY( EditAnywhere , Category = "Visual Effects")
	UParticleSystem* HitParticles ;
};
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
class UPawnSensingComponent;

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
	void CheckPatrolTarget();
	void CheckCombatTarget();

protected:
	virtual void BeginPlay() override;

	/* 
		Montage Play Functions
	*/
	void PlayHitReactMonatge(const FName& SectionName); // Also used const referenced here to avoid copying
	void PlayDeathMontage() ; // Die for Stephen

	UPROPERTY(BlueprintReadOnly) 
	EDeathPose DeathPose = EDeathPose :: EDP_Alive;

	bool InTargetRange( AActor* Target , double Radius ) ;
	void MoveToTarget( AActor* Target ) ;
	AActor* ChoosePatrolTarget( ) ;
	UFUNCTION( ) // Since it has to be bind with the delegate ; so make it UFUNCTION()
	void PawnSeen(APawn* SeenPawn) ;

	UPROPERTY( BlueprintReadOnly )
	EEnemyState EnemyState = EEnemyState :: EES_Patrolling ;

private:

	/* 
	Components 
	*/

	UPROPERTY( VisibleAnywhere )
	UAttributeComponent* Attributes;

	UPROPERTY( VisibleAnywhere )
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY( VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

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

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.0f;

	/* 
	Naviagation 
	*/

	UPROPERTY( )
	class AAIController* EnemyController ;

	// Current Patrol Target
	UPROPERTY( EditInstanceOnly , Category = "AI Navigation" , BlueprintReadWrite , meta = (AllowPrivateAccess = "true" ) )
	AActor* PatrolTarget ;

	UPROPERTY( EditInstanceOnly, Category = "AI Navigation" )
	TArray< AActor* > PatrolTargets;

	UPROPERTY( EditAnywhere )
	double PatrolRadius = 200.0f ; // This radius is to avoid collision with the patrol location.

	FTimerHandle PatrolTimer ;
	void PatrolTimerFinished( ) ;

	UPROPERTY( EditAnywhere, Category = "AI Navigation" )
	float WaitMin = 4.0f ;

	UPROPERTY( EditAnywhere , Category = "AI Navigation" )
	float WaitMax = 10.0f ;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter 
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser ) override ;
	virtual void Destroyed() override ; 

protected:
	virtual void BeginPlay() override;

	/* 
		Montage Play Functions
	*/
	virtual int32 PlayDeathMontage() override ; 
	virtual void Die() override ;

	UPROPERTY( EditAnywhere )
	float DeathLifeSpan = 10.0f ;

	UPROPERTY(BlueprintReadOnly) 
	TEnumAsByte<EDeathPose> DeathPose ;

	void CheckPatrolTarget();
	void CheckCombatTarget();
	bool InTargetRange( AActor* Target , double Radius ) ;
	void MoveToTarget( AActor* Target ) ;
	AActor* ChoosePatrolTarget( ) ;
	UFUNCTION( ) // Since it has to be bind with the delegate ; so make it UFUNCTION()
	void PawnSeen(APawn* SeenPawn) ;

	UPROPERTY( BlueprintReadOnly )
	EEnemyState EnemyState = EEnemyState :: EES_Patrolling ;

	virtual void Attack() override ;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override ;

private:
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY( VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY( EditAnywhere ) ;
	TSubclassOf<class AWeapon> WeaponClass ;

	UPROPERTY( ) // To make sure its initialize to nullptr not garbage.
	AActor* CombatTarget ;

	UPROPERTY( EditAnywhere ) 
	double CombatRadius = 500.0f ;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.0f ;

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

	/* AI Behaviour */
	void HideHealthBar() ;
	void ShowHealthBar();
	void LoseInterest() ;
	void StartPatrolling() ;
	void ChaseTarget() ;
	void ClearPatrolTimer() ;
	void ClearAttackTimer() ;

	bool IsOutsideRadius( float Radius ) ;
	bool IsInsideAttackRadius() ;
	bool IsChasing() const ;
	bool IsAttacking() const ;
	bool IsDead() const ;
	bool IsEngaged() const;

	/* Combat */
	FTimerHandle AttackTimer ;
	void StartAttackTimer( ) ;

	UPROPERTY( EditAnywhere , Category = "Combat" )
	float AttackMin = 0.5f ;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0f ;

	UPROPERTY( EditAnywhere , Category = "Combat" )
	float PatrollingSpeed = 175.0f ;
	UPROPERTY( EditAnywhere, Category = "Combat" )
	float ChasingSpeed = 400.0f ; 

};

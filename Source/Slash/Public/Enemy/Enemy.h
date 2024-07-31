// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;
class ASoul ;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter 
{
	GENERATED_BODY()

public:
	AEnemy();
	/* <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/* <AActor> */

	/* <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker) override;
	/* <IHitInterface> */

protected:
	/* <AActor> */
	virtual void BeginPlay() override;
	/* <AActor> */

	/* <ABaseCharacter> */
	virtual void Die() override;
	void SpawnSoul();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	/* <ABaseCharacter> */
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState :: EES_Patrolling;

private:
	/*  AI-Behaviour */
	void ShowHealthBar();
	void HideHealthBar();
	UFUNCTION() 
	void PawnSeen(APawn* SeenPawn); // Callback for UPawnSeen in UPawnSensingComponent
		/* Patrol-Target*/
	void InitializeEnemy();
	void StartPatrolling();
	AActor* ChoosePatrolTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void ClearPatrolTimer();
	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);
	void SpawnDefaultWeapon();
		/* Combat-Target*/
	void CheckCombatTarget();
	void LoseInterest();
	void ChaseTarget();
	void StartAttackTimer();
	void ClearAttackTimer();

	bool IsOutsideRadius(float Radius);
	bool IsInsideAttackRadius();
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsDead() const;
	bool IsEngaged() const;
	/* <AI-Behaivour> */

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere);
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1500.0f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.0f;

	/*
	Naviagation
	*/
	UPROPERTY()
	class AAIController* EnemyController;

	// Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray< AActor* > PatrolTargets;

	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.0f; // This radius is to avoid collision with the patrol location.

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 4.0f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed = 175.0f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed = 400.0f;

	/* Combat */
	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.0f;
	UPROPERTY(EditAnywhere)
	float DeathLifeSpan = 10.0f;

	FTimerHandle RestablingTimer;
	void IfHit_Restablize();
	UPROPERTY( EditAnywhere , Category = "Combat" )
	float RestablingDelayMin = 2.0f ;
	float RestablingDelayMax = 5.0f ;
	UPROPERTY( EditAnywhere , Category = "Combat" )
	TSubclassOf<ASoul> SoulClass ;

};

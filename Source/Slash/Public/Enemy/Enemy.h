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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser ) override ;
	virtual void Destroyed() override ; 
	void CheckPatrolTarget();
	void CheckCombatTarget();

protected:
	virtual void BeginPlay() override;

	/* 
		Montage Play Functions
	*/
	virtual void PlayDeathMontage() override ; 

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

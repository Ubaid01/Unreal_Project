// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh() -> SetGenerateOverlapEvents(true);
	GetMesh() -> SetCollisionObjectType( ECollisionChannel :: ECC_WorldDynamic ) ; 
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Visibility , ECollisionResponse :: ECR_Block );
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore);
	GetCapsuleComponent() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget -> SetupAttachment(GetRootComponent());

	GetCharacterMovement() -> bOrientRotationToMovement = true ;
	bUseControllerRotationPitch = false ;
	bUseControllerRotationYaw = false ;
	bUseControllerRotationRoll = false ;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>( TEXT("PawnSensing") ) ;
	PawnSensing -> SightRadius = 4000.0f ;
	PawnSensing -> SetPeripheralVisionAngle( 45.0f ) ;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay() ;

	if ( PawnSensing ) 
	{
		PawnSensing -> OnSeePawn.AddDynamic( this , &AEnemy::PawnSeen ) ;
	}
	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( IsDead() ) return ;

	if ( EnemyState > EEnemyState :: EES_Patrolling ) // If EnemyState is more serious than Patrolling
	{
		CheckCombatTarget();
	}
	else 
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage( DamageAmount ) ;
	CombatTarget = EventInstigator -> GetPawn( ) ; // No need to cast as we can store address of an operator of child class in its parent class.

	if ( IsInsideAttackRadius() ) 
	{
		EnemyState = EEnemyState::EES_Attacking ;
	}
	else if ( IsOutsideRadius(AttackRadius) )
	{
		ChaseTarget() ; // So we don't need to compute distance multiple times.
	}
	return DamageAmount ;
}

void AEnemy::Destroyed()
{
	if ( EquippedWeapon ) 
	{
		EquippedWeapon -> Destroy() ;
	}
}

//void AEnemy::IfHit_Restablize()
//{
//	if ( EnemyState == EEnemyState::EES_Attacking )
//		EnemyState = EEnemyState::EES_Chasing;
//	
//	GetWorld() -> GetTimerManager().ClearTimer(RestablingTimer);
//}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Attacker )
{
	Super :: GetHit_Implementation( ImpactPoint , Attacker ) ;
	//const float RestablingDelay = FMath::RandRange(RestablingDelayMin, RestablingDelayMax) ;
	//GetWorld() -> GetTimerManager().SetTimer( RestablingTimer , this, &AEnemy::IfHit_Restablize, RestablingDelay ) ; 	// Start a delay timer for enemy to restable itself if hit while attacking.

	if( ! IsDead() ) 
		ShowHealthBar() ;
	ClearPatrolTimer() ;
	ClearAttackTimer() ;
	SetWeaponCollisionEnabled(ECollisionEnabled :: NoCollision);
	StopAttackMontage() ;

	if ( IsInsideAttackRadius() && !IsDead() )
		StartAttackTimer() ;
}

void AEnemy::Die_Implementation()
{
	EnemyState = EEnemyState::EES_Dead;
	Super :: Die_Implementation() ; // Now must call this Super else it will result in an Infinitie Loop. 
	ClearAttackTimer() ;
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan( DeathLifeSpan ) ;
	GetCharacterMovement() -> bOrientRotationToMovement = false ; // Set Rotation to false as after dying enemy was rotating its body to adjust themselves.
	SetWeaponCollisionEnabled( ECollisionEnabled :: NoCollision ) ;
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();

	if ( World && SoulClass && Attributes )
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, 125.0f);
		ASoul* SpawnedSoul = World -> SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation() ) ;
		if ( SpawnedSoul )
		{
			SpawnedSoul -> SetSouls( Attributes -> GetSouls() ) ;
			SpawnedSoul -> SetOwner( this ) ;
		}
	}
}

void AEnemy::Attack()
{
	Super :: Attack() ; // Now 1st call super version and after that check before engaging
	if ( CombatTarget == nullptr ) return ;
	EnemyState = EEnemyState::EES_Engaged ; 
	PlayAttackMontage() ;
}

bool AEnemy::CanAttack() 
{
	const bool bCanAttack = IsInsideAttackRadius() && !IsAttacking() 
							&& !IsDead() && !IsEngaged() ;
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState ;
	CheckCombatTarget() ;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super :: HandleDamage(DamageAmount) ;
	if ( Attributes && HealthBarWidget ) 
	{
		HealthBarWidget -> SetHealthPercent( Attributes -> GetHealthPercent() ) ;
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget -> SetVisibility( true );
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget -> SetVisibility(false);
	}
}

void AEnemy::PawnSeen( APawn* SeenPawn )
{
	// Check Not Chasing To avoid delegate call multiple times.
	const bool bShouldChaseTarget = ( EnemyState != EEnemyState::EES_Dead ) && ( EnemyState != EEnemyState :: EES_Chasing ) && ( EnemyState < EEnemyState::EES_Attacking ) && (SeenPawn -> ActorHasTag(FName("EngageableTarget") ) && ( ! SeenPawn -> ActorHasTag(FName("Dead") ) ) ) ;

	if ( bShouldChaseTarget ) 
	{
		CombatTarget = SeenPawn ;
		ClearPatrolTimer();
		ChaseTarget() ;
	}
}

void AEnemy :: InitializeEnemy() 
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement() -> MaxWalkSpeed = PatrollingSpeed ;
	MoveToTarget(PatrolTarget);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray < AActor* > ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget) // 1st check whether it is similiar to previous target or not.
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 SelectionValue = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[SelectionValue] ; // Now change the PatrolTarget with this new target.
	}

	return nullptr ; // If empty TArray
}

void AEnemy::CheckPatrolTarget()
{
	if ( InTargetRange( PatrolTarget, PatrolRadius) )
	{
		PatrolTarget = ChoosePatrolTarget( ) ;
		const float WaitTime = FMath :: RandRange( PatrolWaitMin , PatrolWaitMax ) ;
		GetWorldTimerManager( ).SetTimer( PatrolTimer, this, &AEnemy :: PatrolTimerFinished, WaitTime ); // After 5 secs ; our callback delegate  will be used on this ( same actor ) .
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget( PatrolTarget ) ;
}

void AEnemy::ClearPatrolTimer() 
{
	GetWorldTimerManager().ClearTimer(PatrolTimer); // Since timer has been cleared so delegate will not be called and enemy will not move to next patrol point.
}

void AEnemy::MoveToTarget( AActor* Target )
{
	if (EnemyController == nullptr || Target == nullptr) return ;
	FAIMoveRequest MoveRequest; // struct
	MoveRequest.SetGoalActor( Target );
	MoveRequest.SetAcceptanceRadius( AcceptanceRadius ) ;
	EnemyController -> MoveTo( MoveRequest ); // NavMesh was optional
	FNavPathSharedPtr NavPath;
	EPathFollowingRequestResult::Type Result = EnemyController->MoveTo(MoveRequest, &NavPath);

	if (Result != EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveTo request failed! Result: %d"), Result);
	}
	else
	{
		// Optional: Log path points for debugging
		if (NavPath && NavPath->GetPathPoints().Num() > 0)
		{
			for (auto& Point : NavPath->GetPathPoints())
			{
				UE_LOG(LogTemp, Warning, TEXT("Path Point: %s"), *Point.Location.ToString());
			}
		}
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if ( Target == nullptr ) return false ;

	const double DistanceToTarget = (Target -> GetActorLocation( ) - GetActorLocation( ) ) . Size( ) ; // Size() or Length() to get vector length.
	return DistanceToTarget <= Radius ; // If true ; Enemy has reached within the target bound.
}

void AEnemy::SpawnDefaultWeapon()
{

	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World -> SpawnActor<AWeapon>(WeaponClass); // Since we are not providing location and rotation so we will need to specify ; we will have to attach it .
		DefaultWeapon -> Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::CheckCombatTarget()
{
	if ( IsOutsideRadius( CombatRadius ) ) 
	{
		ClearAttackTimer() ;
		LoseInterest( ) ;
		if ( !IsEngaged() ) 
			StartPatrolling( ) ;

	}
	else if ( IsOutsideRadius( AttackRadius ) && !IsChasing() ) // Since this function is called in Tick everyframe so only set it once to avoid bugs.
	{
		ClearAttackTimer() ;
		if( !IsEngaged() )
			ChaseTarget( ) ;
	}
	else if ( CanAttack( ) )
	{
		//ClearAttackTimer(); // As we don't want this to be called each frame even though in all conditions ; so avoid spamming by only checking in cases. AttackPreviousTimer.
		StartAttackTimer( ) ;
	}
}

void AEnemy::LoseInterest()
{
	// Disable HealthBar widget and UnSee the CombatTarget which is SlashCharacter.
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy :: ChaseTarget() 
{
	EnemyState = EEnemyState:: EES_Chasing;
	GetCharacterMovement() -> MaxWalkSpeed = ChasingSpeed;
	UE_LOG(LogTemp, Warning, TEXT("Current Acceptance Radius: %f"), AcceptanceRadius);
	MoveToTarget(CombatTarget);
}

void AEnemy :: StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking ;
	const float AttackTime = FMath :: RandRange( AttackMin, AttackMax ) ;
	GetWorldTimerManager() . SetTimer( AttackTimer, this, &AEnemy :: Attack, AttackTime ) ;
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer); 
}

bool AEnemy::IsOutsideRadius( float Radius ) 
{
	return !InTargetRange(CombatTarget, Radius) ; // As it is checking a non-const function so can't make const.
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius) ;
}

bool AEnemy::IsChasing() const
{
	return EnemyState == EEnemyState::EES_Chasing ;
}

bool AEnemy:: IsAttacking() const 
{
	return EnemyState == EEnemyState::EES_Attacking ;
}

bool AEnemy::IsDead() const
{
	return EnemyState == EEnemyState::EES_Dead ;
}

bool AEnemy:: IsEngaged() const
{
	return EnemyState == EEnemyState :: EES_Engaged ;
}

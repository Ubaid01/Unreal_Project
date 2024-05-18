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
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/GameplayStatics.h"
#include "Slash/DebugMacros.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh() -> SetGenerateOverlapEvents(true);
	GetMesh() -> SetCollisionObjectType( ECollisionChannel :: ECC_WorldDynamic ) ; 
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Visibility , ECollisionResponse :: ECR_Block );
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore);
	GetCapsuleComponent() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore); 

	Attributes = CreateDefaultSubobject<UAttributeComponent>( TEXT("Attributes") ) ; // Since it don't have a mesh or location so no need to attach it to root component.

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>( TEXT("HealthBar") ) ;
	HealthBarWidget -> SetupAttachment( GetRootComponent() ) ;

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
	Super::BeginPlay();
	
	if ( HealthBarWidget )
	{
		HealthBarWidget -> SetVisibility( false ) ; 
	}

	EnemyController = Cast<AAIController>( GetController() );
	MoveToTarget( PatrolTarget ) ;

	if ( PawnSensing ) 
	{
		PawnSensing -> OnSeePawn.AddDynamic( this , &AEnemy::PawnSeen ) ;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( EnemyState > EEnemyState :: EES_Patrolling ) // If EnemyState is more serious than Patrolling
	{
		CheckCombatTarget();
	}
	else 
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckCombatTarget()
{

	if (!InTargetRange( CombatTarget , CombatRadius ) ) // If we are not in range of combat target.
	{
		// Disable HealthBar widget and Unsee the CombatTarget which is SlashCharacter.
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget -> SetVisibility(false);
		}
		// Losing interest
		EnemyState = EEnemyState::EES_Patrolling ;
		GetCharacterMovement( ) -> MaxWalkSpeed = 125.0f ; 
		MoveToTarget( PatrolTarget ) ;
	}
	else if ( ! InTargetRange( CombatTarget , AttackRadius ) && EnemyState != EEnemyState::EES_Chasing ) // Since this function is called in Tick everyframe so only set it once to avoid bugs.
	{
		// Outside AttackRange ( Chase Character )
		EnemyState = EEnemyState:: EES_Chasing ; 
		GetCharacterMovement() -> MaxWalkSpeed = 300.0f ;
		MoveToTarget( CombatTarget ) ;
	}
	else if ( InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking ) 
	{
		// Inside AttackRange ; attack character.
		EnemyState = EEnemyState::EES_Attacking ;
		// TODO : AttackMontage
	}
}

void AEnemy::CheckPatrolTarget()
{
	if ( InTargetRange( PatrolTarget, PatrolRadius) )
	{
		PatrolTarget = ChoosePatrolTarget( ) ;
		const float WaitTime = FMath :: RandRange( WaitMin , WaitMax ) ;
		GetWorldTimerManager( ).SetTimer( PatrolTimer, this, &AEnemy :: PatrolTimerFinished, WaitTime ); // After 5 secs ; our callback delegate  will be used on this ( same actor ) .
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if ( Target == nullptr ) return false ;

	const double DistanceToTarget = (Target -> GetActorLocation( ) - GetActorLocation( ) ) . Size( ) ; // Size() or Length() to get vector length.
	return DistanceToTarget <= Radius ;
}

void AEnemy::MoveToTarget( AActor* Target )
{
	if (EnemyController == nullptr || Target == nullptr) return ;
	FAIMoveRequest MoveRequest; // struct
	MoveRequest.SetGoalActor( Target );
	MoveRequest.SetAcceptanceRadius( 15.0f ) ;
	EnemyController -> MoveTo( MoveRequest ); // NavMesh was optional
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

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget( PatrolTarget ) ;
}

void AEnemy::PawnSeen( APawn* SeenPawn )
{
	if (EnemyState == EEnemyState::EES_Chasing) return ; // To avoid delegate call multiple times.
	if ( SeenPawn -> ActorHasTag( FName("SlashCharacter") ) )
	{
		if ( EnemyState != EEnemyState::EES_Attacking ) // Since if we are attacking no need to chase OR call PawnSeen.
		{
			EnemyState = EEnemyState::EES_Chasing ;
		}
		GetWorldTimerManager( ).ClearTimer( PatrolTimer ) ; // Since timer has been cleared so delegate will not be called and enemy will not move to next patrol point.
		GetCharacterMovement( ) -> MaxWalkSpeed = 300.0f ;
		CombatTarget = SeenPawn ;
		MoveToTarget( CombatTarget ) ;
	}
}

void AEnemy::PlayHitReactMonatge( const FName& SectionName )
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance -> Montage_Play(HitReactMontage);
		AnimInstance -> Montage_JumpToSection( SectionName, HitReactMontage ) ;
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if ( HealthBarWidget ) 
	{
		HealthBarWidget -> SetVisibility( true ) ;
	}
	if ( Attributes && Attributes -> IsAlive( ) ) 
	{
		DirectionalHitReact(ImpactPoint);
	}
	else 
	{
		PlayDeathMontage();
	}

	if ( HitSound ) 
	{
		UGameplayStatics :: PlaySoundAtLocation(this, HitSound, ImpactPoint); 
	}

	if ( HitParticles && GetWorld( ) )
	{
		UGameplayStatics :: SpawnEmitterAtLocation( GetWorld(), HitParticles, ImpactPoint ) ; // Here we don't require UObjectWorldContext but UWorld pointer itself.
	}

}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Elevation so that parallel to the ground ( Different due to different animations ) .
	const FVector ImpactLower(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLower - GetActorLocation()).GetSafeNormal();
	const double CosTheta = FVector :: DotProduct(Forward, ToHit);
	double Theta = FMath :: Acos(CosTheta);
	Theta = FMath :: RadiansToDegrees(Theta);

	// If CrossProduct points -ve ; vector points downward ;
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0.0f) {
		Theta *= -1;
	}

	FName Section("FromBack"); // Pre-initialize with the Back
	if (Theta >= -45.0f && Theta < 45.0f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.0f && Theta < -45.0f) {
		const int32 Selection = FMath :: RandRange(0, 1);
		switch (Selection)
		{
		case 0:
			Section = FName("FromLeft");
			break;
		case 1:
			Section = FName("FromLeft_1");
			break;
		default:
			break;
		}
	}
	else if (Theta >= 45.0f && Theta < 135.0f) {
		const int32 Selection = FMath :: RandRange(0, 1);
		switch (Selection)
		{
		case 0:
			Section = FName("FromRight");
			break;
		case 1:
			Section = FName("FromRight_1");
			break;
		default:
			break;
		}
	}
	PlayHitReactMonatge(Section);
}

void AEnemy::PlayDeathMontage()
{
	// TO PLAY DeathMontage
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance -> Montage_Play(DeathMontage) ;
		const int32 Selection = FMath :: RandRange( 0 , 6 ); 
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose :: EDP_Death1;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose :: EDP_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			DeathPose = EDeathPose :: EDP_Death3;
			break;
		case 3:
			SectionName = FName("Death4");
			DeathPose = EDeathPose :: EDP_Death4;
			break;
		case 4:
			SectionName = FName("Death5");
			DeathPose = EDeathPose :: EDP_Death5;
			break;
		case 5:
			SectionName = FName("Death6");
			DeathPose = EDeathPose :: EDP_Death6;
			break;
		case 6:
			SectionName = FName("Death7");
			DeathPose = EDeathPose :: EDP_Death7;
			break;
		default:
			break;
		}
		AnimInstance -> Montage_JumpToSection(SectionName, DeathMontage);
	}
	if ( HealthBarWidget )
	{
		HealthBarWidget -> SetVisibility( false ) ; // Set Visibility of Widget to false ; for how many seconds dead enemy is stick around.
	}
	SetLifeSpan( 60.0f ) ; // Reset the body for 1 minute.
	GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled :: NoCollision) ;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if ( Attributes && HealthBarWidget ) 
	{
		Attributes -> ReceiveDamage( DamageAmount ) ;
		HealthBarWidget -> SetHealthPercent( Attributes -> GetHealthPercent( ) ) ; 
	}
	CombatTarget = EventInstigator -> GetPawn( ) ; // No need to cast as we can store address of an operator of child class in its parent class.

	// TO Make Enemy Chase when hit even from back
	EnemyState = EEnemyState::EES_Chasing ;
	GetCharacterMovement() -> MaxWalkSpeed = 300.0f ;
	MoveToTarget( CombatTarget ) ;

	return DamageAmount ;
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

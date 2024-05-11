// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"


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

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if ( HealthBarWidget )
	{
		HealthBarWidget -> SetVisibility( false ) ; 
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ( CombatTarget ) 
	{
		const double DistanceToTarget = ( CombatTarget -> GetActorLocation() - GetActorLocation() ) .Size(); // Size() or Length() to get vector length.

		if ( DistanceToTarget > CombatRadius ) 
		{
			CombatTarget = nullptr ;
			if ( HealthBarWidget ) 
			{
				HealthBarWidget -> SetVisibility( false ) ;
			}
		}
	}
}

void AEnemy::PlayHitReactMonatge( const FName& SectionName )
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance -> Montage_Play(HitReactMontage);
		AnimInstance -> Montage_JumpToSection(SectionName, HitReactMontage);
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
		const int32 Selection = FMath :: RandRange( 0 , 6 ); // Is used to play different Montages Slots at random
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
			break; // Will give error even if last break NOT given
		}
		AnimInstance -> Montage_JumpToSection(SectionName, DeathMontage);
	}
	GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled :: NoCollision) ;
	SetLifeSpan( 60.0f ) ; // Reside the body for 1 minute.
	if ( HealthBarWidget )
	{
		HealthBarWidget -> SetVisibility( false ) ; // Set Visibility of Widget to false ; for how many seconds dead enemy is stick around.
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if ( Attributes && HealthBarWidget ) 
	{
		Attributes -> ReceiveDamage( DamageAmount ) ;
		HealthBarWidget -> SetHealthPercent( Attributes -> GetHealthPercent( ) ) ; // Already provided Function
	}
	CombatTarget = EventInstigator -> GetPawn( ) ; // No need to cast as we can store address of an operator of child class in its parent class.

	return DamageAmount ;
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

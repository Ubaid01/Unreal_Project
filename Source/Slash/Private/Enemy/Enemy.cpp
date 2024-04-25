// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Slash/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh() -> SetGenerateOverlapEvents(true);
	GetMesh() -> SetCollisionObjectType( ECollisionChannel :: ECC_WorldDynamic ) ; 
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Visibility , ECollisionResponse :: ECR_Block );
	GetMesh() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore);
	GetCapsuleComponent() -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore); 

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	DirectionalHitReact(ImpactPoint);

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

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

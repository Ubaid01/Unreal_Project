// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Slash/DebugMacros.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes")); // Since it don't have a mesh or location so no need to attach it to root component.
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint , AActor* Attacker )
{
	if ( IsAttributeAlive() )
	{
		DirectionalHitReact( Attacker -> GetActorLocation() ) ;
	}
	else 
		Die() ;

	PlayHitSound( ImpactPoint ) ;
	SpawnHitParticles( ImpactPoint ) ;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::Attack()
{
	if ( CombatTarget && CombatTarget -> ActorHasTag( FName("Dead") ) )
	{
		CombatTarget = nullptr ;
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontage( AttackMontage, AttackMontageSections ) ;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontage( DeathMontage , DeathMontageSections ); // Returned int32 as we had to setup death-pose depending on death-montage also.
	TEnumAsByte<EDeathPose> Pose(Selection); // This Byte Wrapper will set the value of template to parameter ; here Pose to Selection .

	if (Pose < EDeathPose :: EDP_MAX)
		DeathPose = Pose;

	return Selection ;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance() ;
	if ( AnimInstance ) 
	{
		AnimInstance -> Montage_Stop( 0.25, AttackMontage ) ;
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent() -> SetCollisionEnabled(ECollisionEnabled :: NoCollision) ;
}

int32 ABaseCharacter::PlayRandomMontage(UAnimMontage* Montage, const TArray<FName>& SectionNames )
{
	if (SectionNames.Num() <= 0) return -1 ;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath :: RandRange(0, MaxSectionIndex);
	PlayMontageSection( Montage , SectionNames [Selection] ) ;
	return Selection ;
}

void ABaseCharacter::Die()
{
	Tags.Add( FName("Dead") ) ;
	PlayDeathMontage();
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance() ;
	if (AnimInstance && Montage) 
	{
		AnimInstance -> Montage_Play( Montage ) ;
		AnimInstance -> Montage_JumpToSection( SectionName , Montage ) ;
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint )
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

void ABaseCharacter::PlayHitReactMonatge(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh() -> GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance -> Montage_Play(HitReactMontage);
		AnimInstance -> Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics :: PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if ( HitParticles && GetWorld() )
	{
		UGameplayStatics :: SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint); // Here we don't require UObjectWorldContext but UWorld pointer itself.
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount )
{
	if ( Attributes ) // Since all class shall not have HealthBarWidget so make it virtual and apply in that child class.
	{
		Attributes -> ReceiveDamage( DamageAmount ) ;
	}
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::CanAttack() 
{
	return false;
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if( CombatTarget == nullptr )
		return FVector() ;

	const FVector CombatTargetLocation = CombatTarget -> GetActorLocation( ) ;
	const FVector Location = GetActorLocation() ; // Enemy's Location
	FVector TargetDistance = ( Location - CombatTargetLocation ) . GetSafeNormal() ; // Normalize the vector also as we have to scale by our warp value.

	TargetDistance *= WarpTargetDistance ;
	return CombatTargetLocation + TargetDistance ;

}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if ( CombatTarget ) 
	{
		return CombatTarget -> GetActorLocation() ;
	}
	return FVector();
}

bool ABaseCharacter::IsAttributeAlive()
{
	return Attributes && Attributes -> IsAlive() ;
}

void ABaseCharacter::DisbaleMeshCollision()
{
	GetMesh() -> SetCollisionEnabled(ECollisionEnabled :: NoCollision); 
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if ( EquippedWeapon && EquippedWeapon -> GetWeaponBox() )
	{
		EquippedWeapon -> GetWeaponBox() -> SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon -> IgnoreActors.Empty(); // Empty the IgnoreActors for each time the notify is ON-OFF
	}
}

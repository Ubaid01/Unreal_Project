// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes")); // Since it don't have a mesh or location so no need to attach it to root component.
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PlayAttackMontage()
{
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

void ABaseCharacter::PlayDeathMontage()
{
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::CanAttack() const
{
	return false;
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if ( EquippedWeapon && EquippedWeapon -> GetWeaponBox() )
	{
		EquippedWeapon -> GetWeaponBox() -> SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon -> IgnoreActors.Empty(); // Empty the IgnoreActors for each time the notify is ON-OFF
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"
#include "Items/Potion.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false ;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::RegenerateStamina(float DeltaTime)
{
	Stamina = FMath :: Clamp(Stamina + (StaminaRegenerate * DeltaTime), 0.0f, MaxStamina);
}

void UAttributeComponent::UpgradeHealth(const APotion* Potion)
{
	Health = FMath :: Clamp(Health + Potion -> GetPoints() , 0.0f, MaxHealth);
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath :: Clamp( Health - Damage, 0.0f, MaxHealth ) ; // Clamp the value so that it never goes below 0.
}

void UAttributeComponent::UseStamina( float StaminaCost )
{
	Stamina = FMath :: Clamp( Stamina - StaminaCost , 0.0f, MaxStamina ) ;
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth ;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina ;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.0f ;
}

void UAttributeComponent::AddToGold(int32 AmonutOfGold)
{
	Gold += AmonutOfGold;
}

void UAttributeComponent::AddToSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls ;
}


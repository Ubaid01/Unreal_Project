// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath :: Clamp( Health - Damage, 0.0f, MaxHealth ) ; // Clamp the value so that it never goes below 0.
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth ;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.0f ;
}


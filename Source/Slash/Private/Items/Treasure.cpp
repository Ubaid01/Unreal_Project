// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/SlashCharacter.h"

void ATreasure ::  OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// No need to call Super as we are not attaching the Weapon when overlapping coin also.
	IPickupInterface* PickupInterface = Cast<IPickupInterface>( OtherActor ) ;
	if ( PickupInterface )
	{
		PickupInterface -> AddGold( this ) ;
		SpawnPickupSound() ; // Since now we have similiar name USoundBase in AItem so removed from here.

		Destroy(); // To destroy the item at the overlap.
	}

}


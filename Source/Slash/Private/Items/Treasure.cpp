// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure ::  OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// No need to call Super as we are not attaching the Weapon when overlapping coin also.
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharacter)
	{
		if (PickupSound) 
		{
			UGameplayStatics :: PlaySoundAtLocation( this, PickupSound, GetActorLocation( ) ) ;
		}

		Destroy(); // To destroy the item at the overlap.
	}

}


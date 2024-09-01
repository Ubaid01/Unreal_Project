// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Potion.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void APotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.0f, 0.0f, DeltaTime * DriftRate);
		AddActorWorldOffset(DeltaLocation);
	}
}

void APotion::BeginPlay()
{
	Super::BeginPlay() ;

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.0f, 0.0f, 2000.0f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	FHitResult HitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

	DesiredZ = HitResult.ImpactPoint.Z + MinFromGround ;
}

void APotion::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if ( PickupInterface )
	{
		PickupInterface -> AddHealth( this ) ;

		SpawnPickupEffect(); // Moved in if-check so that enemy while dying does not play Sound or Effects
		//SpawnPickupSound();
		//Destroy(); We only need to destroy if equipped else not.
	}
}

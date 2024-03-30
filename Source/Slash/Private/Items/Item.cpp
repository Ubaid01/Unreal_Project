// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include<Slash/DebugMacros.h>


AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super :: BeginPlay();

	UWorld* World = GetWorld(); 
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector() ; // It gets the forward (x-axis as we want line to scale horizontally) unit vector (which means of 1 unit of UE i.e. 1cm ) .

	MY_DEBUG_SPHERE(Location)
	DRAW_VECTOR(Location, Location + Forward * 100.0f)
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


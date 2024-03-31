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

}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Running_Time += DeltaTime; 
	float Delta_Z = 2.0f * FMath :: Sin( Running_Time * 2.0f ) ; // sin period = 2*pi / k ;
	AddActorWorldOffset(FVector(0.0f, 0.0f, Delta_Z));
	DRAW_SPHERE_SINGLE_FRAME( GetActorLocation( ) )
	DRAW_VECTOR_SINGLE_FRAME( GetActorLocation( ) , GetActorLocation( ) + GetActorForwardVector( ) * 100.0f ) 
	
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakables/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false ; // As we don't want to update this over time period.

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>( TEXT("GeometryCollection") ) ;

	SetRootComponent( GeometryCollection ) ; // As Geometry Collection Component is derived from USceneComponent so can use it as RootComponent also.

	GeometryCollection -> SetGenerateOverlapEvents( true ) ; 
	GeometryCollection -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Camera, ECollisionResponse :: ECR_Ignore ) ; // To ignore Camera zoom if some piece of actor flies towards character.
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	//GeometryCollection -> EnableClustering = false;
}


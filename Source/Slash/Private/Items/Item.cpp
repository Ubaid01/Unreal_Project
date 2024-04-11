// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include<Slash/DebugMacros.h>
#include<DrawDebugHelpers.h>


AItem::AItem() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent")); // Unreal Engine uses factory functions for pointers rather than "new".
	RootComponent = ItemMesh ; // Here we replace the Rootcomponent with ItemMesh for DefaultSceneRootComponent so Unreal's garbage collection system will see that since RootComponent points nothing so will delete its pointer automatically.

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super :: BeginPlay();

}

float AItem::TransformedSine( )
{
	return Amplitude * FMath :: Sin( Running_Time * Time_Constant ); // sin period = 2*pi / k 
}

float AItem::TransformedCosine()
{
	return Amplitude * FMath :: Cos(Running_Time * Time_Constant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Running_Time += DeltaTime; 

	//FVector Vector = FVector( 0.0f , 0.0f , TransformedSine( ) );

	//AddActorWorldOffset(Vector);

	//FRotator Rotation = FRotator( TransformedSine( ) , TransformedCosine(), 0.0f) ;
    //AddActorWorldRotation(Rotation); // Rotate the actor

	//DRAW_VECTOR_SINGLE_FRAME( GetActorLocation( ) , GetActorLocation() + GetActorForwardVector( ) * 100.0f )

}


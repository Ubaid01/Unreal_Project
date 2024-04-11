// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include<Slash/DebugMacros.h>
#include<DrawDebugHelpers.h>
#include "Components/SphereComponent.h"

AItem::AItem() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent")); // Unreal Engine uses factory functions for pointers rather than "new".
	RootComponent = ItemMesh ; 

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere -> SetupAttachment(GetRootComponent());
	//Sphere -> SetSphereRadius(300.0f);
	

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super :: BeginPlay();

	//Sphere -> OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	//Sphere -> OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap); 

}

float AItem::TransformedSine( )
{
	return Amplitude * FMath :: Sin( Running_Time * Time_Constant ); // sin period = 2*pi / k 
}

float AItem::TransformedCosine()
{
	return Amplitude * FMath :: Cos(Running_Time * Time_Constant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	const FString OtherActorName = OtherActor -> GetName();
	if (GEngine)
	{
		GEngine -> AddOnScreenDebugMessage(0, 5.0f , FColor :: Black, OtherActorName);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FString OtherActorName = FString("Ending Overlap with : " ) + OtherActor -> GetName(); // Appended FString
	if (GEngine)
	{
		GEngine -> AddOnScreenDebugMessage(1, 5.0f, FColor :: Black, OtherActorName);
	}
}

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


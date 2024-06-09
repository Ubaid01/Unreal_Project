// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include<Slash/DebugMacros.h>
#include<DrawDebugHelpers.h>
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem() 
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent")); // Unreal Engine uses factory functions for pointers rather than "new".
	RootComponent = ItemMesh ; 

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere -> SetupAttachment( GetRootComponent() ) ;

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>( TEXT("Embers") ) ;
	ItemEffect -> SetupAttachment(GetRootComponent());

	ItemMesh -> SetCollisionResponseToAllChannels( ECollisionResponse :: ECR_Ignore ) ;
	ItemMesh -> SetCollisionEnabled(ECollisionEnabled :: NoCollision); // Since each time we have to set collision disabled for each mesh ; so setting through C++ ; as we will never want out enemy or character to keep colliding with it.

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super :: BeginPlay();

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
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if ( PickupInterface ) 
	{
		PickupInterface -> SetOverlappingItem(this); 
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if ( PickupInterface )
	{
		PickupInterface -> SetOverlappingItem( nullptr ); 
	}
}

void AItem::SpawnPickupEffect()
{
	if ( PickupEffect )
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}

void AItem::SpawnPickupSound()
{
	if ( PickupSound )
	{
		UGameplayStatics::SpawnSoundAtLocation( this, PickupSound, GetActorLocation() ) ;
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Running_Time += DeltaTime; 

	if (ItemState == EItemState :: EIS_Hovering) 
	{
		AddActorWorldOffset( FVector( 0.0f, 0.0f , TransformedSine( ) ) ) ;
	}

}


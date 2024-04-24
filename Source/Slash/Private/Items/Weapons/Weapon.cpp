// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon:: AWeapon() 
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox")) ;
	WeaponBox -> SetupAttachment( GetRootComponent() ) ;
	WeaponBox -> SetCollisionEnabled(ECollisionEnabled :: NoCollision) ; // Set to No Collision as we need to alter accordingly via attack animations.
	WeaponBox -> SetCollisionResponseToAllChannels(ECollisionResponse :: ECR_Overlap);
	WeaponBox -> SetCollisionResponseToChannel(ECollisionChannel :: ECC_Pawn , ECollisionResponse :: ECR_Ignore );

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart -> SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd -> SetupAttachment(GetRootComponent());

}


void AWeapon::BeginPlay()
{
	Super :: BeginPlay();

	WeaponBox -> OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
	AttachMeshToSocket(InParent, InSocketName) ; 
	ItemState = EItemState::EIS_Equipped ;

	if (EquipSound) 
	{
		UGameplayStatics :: PlaySoundAtLocation( this , EquipSound , GetActorLocation( ) ) ;
	}

	if (Sphere) 
	{
		Sphere -> SetCollisionEnabled( ECollisionEnabled :: NoCollision ) ; // Again setting sphere from Query only to No Collision preset as after attacking it was again doing all overlap events for weapon
	}
}

void AWeapon :: AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName) 
{
	FAttachmentTransformRules TransformRules(EAttachmentRule :: SnapToTarget, EAttachmentRule :: SnapToTarget, EAttachmentRule :: SnapToTarget, true);

	ItemMesh -> AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super :: OnSphereOverlap( OverlappedComponent , OtherActor , OtherComp , OtherBodyIndex , bFromSweep , SweepResult ); 

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super :: OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex );
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart -> GetComponentLocation();
	const FVector End = BoxTraceEnd -> GetComponentLocation();

	TArray <AActor*> ActorsToIgnore ;
	ActorsToIgnore.Add(this); // To Ignore the Actor ( Weapon ) itself

	FHitResult BoxHit; // As passed in by reference so it will be filled just like in blueprint it is used as output node.

	UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(5.0f, 5.0f, 5.0f), BoxTraceStart -> GetComponentRotation(), ETraceTypeQuery :: TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace :: ForDuration, BoxHit , true ) ;

}

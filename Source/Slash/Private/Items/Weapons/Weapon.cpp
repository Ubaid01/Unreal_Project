// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

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

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime) ;
	if ( ItemState == EItemState::EIS_Hovering )
	{
		AddActorWorldRotation(FRotator(0.0f, 360 * FMath :: Sin(0.0075 * Time_Constant), 0.0f) ) ;
	}
}

void AWeapon::BeginPlay()
{
	Super :: BeginPlay();

	WeaponBox -> OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);

}

void AWeapon::Equip( USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInsitigator )
{
	SetOwner( NewOwner ); // Now when weapon is equipped ; it will be linked up with the Owner which can be found by GetOwner() .
	SetInstigator( NewInsitigator );
	AttachMeshToSocket(InParent, InSocketName) ; 
	ItemState = EItemState::EIS_Equipped ;

	DisableSphereCollision();

	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect -> Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere -> SetCollisionEnabled(ECollisionEnabled :: NoCollision);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics :: PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}
}

void AWeapon :: AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName) 
{
	FAttachmentTransformRules TransformRules(EAttachmentRule :: SnapToTarget, EAttachmentRule :: SnapToTarget, EAttachmentRule :: SnapToTarget, true);

	ItemMesh -> AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::DropWeapon( )
{
	DetachFromActor( FDetachmentTransformRules::KeepWorldTransform ) ;
	AddActorWorldOffset( FVector( 100.0f , 100.0f , -2.0f ) ) ;
	SetActorRotation( FRotator::ZeroRotator ) ;
	PlayEquipSound() ;
	ItemState = EItemState::EIS_Hovering ;

	Sphere -> SetCollisionEnabled( ECollisionEnabled::QueryOnly ) ;
	ItemEffect -> Activate() ;
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart -> GetComponentLocation();
	const FVector End = BoxTraceEnd -> GetComponentLocation();

	TArray <AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	for (AActor* Actor : IgnoreActors) // Add the ignore actors for each time the function is called.
	{
		ActorsToIgnore.Add(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle( this, Start, End, BoxTracExtent , BoxTraceStart -> GetComponentRotation(), ETraceTypeQuery :: TraceTypeQuery1, false, ActorsToIgnore, ( ( bShowDebugBox ) ? EDrawDebugTrace :: ForDuration : EDrawDebugTrace :: None ) , BoxHit, true ) ;

	IgnoreActors.AddUnique(BoxHit.GetActor()); // AddUnique to avoid duplication
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface -> Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint , GetOwner( ) );
	}
}
bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return ( GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag( TEXT("Enemy") ) ) || ( GetOwner() -> ActorHasTag(TEXT("EngageableTarget") ) && OtherActor->ActorHasTag(TEXT("EngageableTarget") ) );
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If both actors have enemy tag so return without checking overlap.
	if ( ActorIsSameType( OtherActor ) ) return ;
	FHitResult BoxHit; // As passed in by reference so it will be filled just like in blueprint it is used as output node.
	BoxTrace( BoxHit ) ;
	
	if ( BoxHit.GetActor() ) 
	{
		if ( ActorIsSameType( BoxHit.GetActor( ) ) ) return ;
		UGameplayStatics :: ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator() -> GetController(), this, UDamageType :: StaticClass() ) ;

		ExecuteGetHit(BoxHit);
		CreateFields( BoxHit.ImpactPoint ) ;
	}

}
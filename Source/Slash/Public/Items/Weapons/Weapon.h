// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();
	void Equip( USceneComponent* InParent, FName InSocketName , AActor* NewOwner , APawn* NewInsitigator );
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSockentName);
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox;  }
	TArray<AActor*> IgnoreActors;

protected:

	virtual void BeginPlay() override;
	// UFUNCTION() Macro is also inherited so no need to again write

	UFUNCTION( )
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) ;

	UFUNCTION( BlueprintImplementableEvent ) // We call this function from C++ but implement it from Blueprint so that no definition is required here.
	void CreateFields(const FVector& FieldLoaction);
	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();
	void ExecuteGetHit(FHitResult& BoxHit);

private:

	void BoxTrace( FHitResult& BoxHit ) ;
	bool ActorIsSameType(AActor* OtherActor);

	UPROPERTY( EditAnywhere, Category = "Weapon Properties" )
	FVector BoxTracExtent = FVector( 5.0f ) ;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowDebugBox = false ;

	UPROPERTY( EditAnywhere , Category = "Weapon Properties" )
	USoundBase* EquipSound ;

	UPROPERTY( VisibleAnywhere , Category = "Weapon Properties" )
	UBoxComponent* WeaponBox;

	UPROPERTY( VisibleAnywhere )
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY( EditAnywhere , Category = "Weapon Properties" )
	float Damage = 20.0f ;
};

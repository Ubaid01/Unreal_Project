// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering , 
	EIS_Equipped 
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Sine Parameters" )
	float Amplitude = 2.0f;

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Sine Parameters" )
	float Time_Constant = 2.0;

	UFUNCTION(BlueprintPure)
	float TransformedSine( );

	UFUNCTION( BlueprintPure )
	float TransformedCosine( );

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly )
	UStaticMeshComponent* ItemMesh;

	// As OnBeginOverlap is a dynamic multicast delegate which can be exposed to Blueprint so UFUNCTION() is also required to bind this to delegate
	UFUNCTION( )
	virtual void OnSphereOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ); 

	UFUNCTION( )
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	EItemState ItemState = EItemState :: EIS_Hovering ;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

private:
	UPROPERTY( VisibleInstanceOnly , BlueprintReadOnly , meta = ( AllowPrivateAccess = "true" ) )
	float Running_Time ;

};
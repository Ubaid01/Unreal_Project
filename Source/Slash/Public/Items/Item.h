// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

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

	// As OnBeginOverlap is a dynamic multicast delegate which can be exposed to Blueprint so UFUNCTION() is also required to bind this to delegate
	UFUNCTION( )
	void OnSphereOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult ); 

	UFUNCTION( )
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY( VisibleInstanceOnly , BlueprintReadOnly , meta = ( AllowPrivateAccess = "true" ) )
	float Running_Time ;

	UPROPERTY( VisibleAnywhere )
	UStaticMeshComponent* ItemMesh ;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

};
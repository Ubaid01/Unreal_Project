// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

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

private:
	UPROPERTY( VisibleInstanceOnly , BlueprintReadOnly , meta = ( AllowPrivateAccess = "true" ) )
	float Running_Time ;

	UPROPERTY( VisibleAnywhere )
	UStaticMeshComponent* ItemMesh ;

};
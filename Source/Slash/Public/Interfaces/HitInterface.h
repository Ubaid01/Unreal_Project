// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION( BlueprintNativeEvent ) // No need to mark now as pure virtual as Unreal will see this macro and generate the virtual function
	void GetHit( const FVector& ImpactPoint , AActor* Attacker ) ; // Reference used to avoid making a copy as we don't want to change it here.
};

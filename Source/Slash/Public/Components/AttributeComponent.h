// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void ReceiveDamage( float Damage ) ; 
	float GetHealthPercent( ) ;
	bool IsAlive( ) ;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY( EditAnywhere , Category = "Actor Attributes" )
	float Health ; // Current Health

	UPROPERTY( EditAnywhere, Category = "Actor Attributes" ) // Used EditAnywhere to change it for other Instances like enemies.
	float MaxHealth;
		
};

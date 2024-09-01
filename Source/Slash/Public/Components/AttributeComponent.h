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

	void RegenerateStamina(float DeltaTime);

	FORCEINLINE void ReceiveDamage( float Damage ) ; 
	void UseStamina( float StaminaCost ) ;
	float GetHealthPercent( ) ;
	float GetStaminaPercent( ) ;
	bool IsAlive( ) ;
	void AddToGold( int32 AmonutOfGold ) ;
	void AddToSouls( int32 NumberOfSouls ) ;
	FORCEINLINE int32 GetGold() const { return Gold ; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY( EditAnywhere , Category = "Actor Attributes" )
	float Health ; // Current Health

	UPROPERTY( EditAnywhere, Category = "Actor Attributes" ) // Used EditAnywhere to change it for other Instances like enemies.
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina; // Current Stamina

	UPROPERTY(EditAnywhere, Category = "Actor Attributes") 
	float MaxStamina;

	UPROPERTY( EditAnywhere, Category = "Actor Attributes" )
	int32 Gold ; 
	UPROPERTY( EditAnywhere, Category = "Actor Attributes" )
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.0f ;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenerate = 3.0f;
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API APotion : public AItem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	FORCEINLINE int32 GetPotionGoldCost() const { return PotionGoldCost; }
	FORCEINLINE int32 GetPotionSoulsCost() const { return PotionSoulsCost; }
	FORCEINLINE float GetPoints() const { return PotionPoints; }

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
private:
	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	int32 PotionGoldCost = 25 ;

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	int32 PotionSoulsCost = 30 ;

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float PotionPoints = 25.0f ;

	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float DriftRate = -15.0f;
	UPROPERTY(EditAnywhere, Category = "Potion Properties")
	float MinFromGround = 130.0f;

	double DesiredZ;
	
};

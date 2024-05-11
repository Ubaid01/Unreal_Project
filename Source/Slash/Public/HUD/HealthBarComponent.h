// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"


UCLASS()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent( float Percent ) ; 

private:
	UPROPERTY( ) // UPROPERTY() is good so that the variable is not initialized using garabage but nullptr.
	class UHealthBar* HealthBarWidget ; // Changed so name don't match with UProgressBar HealthBar
};

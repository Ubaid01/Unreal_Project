// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"


UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY( meta = ( BindWidget ) ) // Using this MetaSpecifier our C++ variable will be linked to WBP_Widget but for this the name should be eaxctly matched like ( HealthBar , HealthBar --> ProgressBar ) . If we don't have a matching variable ; so we will have complications.
	class UProgressBar* HealthBar ;

};

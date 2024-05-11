// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarWidget == nullptr) 
	{
		HealthBarWidget = Cast<UHealthBar>( GetUserWidgetObject( ) ) ; // This will return a UUserWidget Object used be this component.But in our case ; since UserWidget is HealthBarWidget so we need to Cast Check.
	}

/* To Avoid Cast Multiple Times in a function ; which is called frequently ; since CASTING is an expensive operation . Compiler will 1st check whether Cast is done or not ; if not it will check Cast to parent class of that object ; if that's not also it will go up the class in hierarchichal manner. So Make a private data member of class ; instead of local to function. */
	if ( HealthBarWidget && HealthBarWidget -> HealthBar ) // Here also 1st check the HealthBarWidget and then its HealthBar as C++ compiler will short-circuit if() condition from left to right.
	{
		HealthBarWidget -> HealthBar -> SetPercent( Percent ) ;
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

class USlashOverlay ;

UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay;  }
protected:
	virtual void BeginPlay() override ;
private:
	UPROPERTY( EditDefaultsOnly , Category = "Slash" )
	TSubclassOf<USlashOverlay> SlashOverlayClass ;
	UPROPERTY( ) // To Initialize as its a good idea for pointers
	USlashOverlay* SlashOverlay ; 

	
};

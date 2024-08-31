// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelCreator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DIGITALTWIN_API ULevelCreator : public UObject
{
	GENERATED_BODY()
public:
    
    UFUNCTION(BlueprintCallable, Category = "MapCreator")
    virtual UWorld* CreateNewLevel(FString ExistingLevelName, FString NewLevelName);

    
};

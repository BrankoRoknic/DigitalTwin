// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CesiumAsset.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumAsset : public UObject
{
	GENERATED_BODY()
public:

	// Member Variables:
	FString fId;
	FString fDisplayName;
	bool fCurrentlyActive;
	FString fUploadDate;
	FString fDataType;
	FString fDataSize;

	// Constructor
	UCesiumAsset();
	void Construct(FString aId, FString aItemName, FString aDate, FString aDataType, FString aDataSize);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Control Method")
	bool IsActiveDifferent(bool aBool);

	virtual void BeginDestroy() override;


	// Getters:
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetId();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetItemName();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetDisplayName();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	bool GetCurrentlyActive();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetUploadDate();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetDataType();
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Get Methods")
	FString GetDataSize();


	// Setters:
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetId(FString aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetDisplayName(FString aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetCurrentlyActive(bool aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetUploadDate(FString aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetDataType(FString aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void SetDataSize(FString aValue);
	UFUNCTION(BlueprintCallable, Category = "CesiumAsset Set Methods")
	void ToggleCurrentlyActive();
};

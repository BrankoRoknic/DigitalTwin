// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumAsset.h"

UCesiumAsset::UCesiumAsset()
{
	fId = "";
	fDisplayName = "";
	fCurrentlyActive = false;
	fUploadDate = "";
	fDataType = "";
	fDataSize = "";
}

void UCesiumAsset::Construct(FString aId, FString aItemName, FString aDate, FString aDataType, FString aDataSize)
{
	fId = aId;
	fDisplayName = aItemName.Replace(TEXT("active"), TEXT(""), ESearchCase::IgnoreCase);
	fCurrentlyActive = aItemName.Contains(TEXT("active"), ESearchCase::IgnoreCase);
	fUploadDate = aDate;
	fDataType = aDataType;
	fDataSize = ByteSizeStringToGb(aDataSize);
}

bool UCesiumAsset::IsActiveDifferent(bool aBool)
{
	return fCurrentlyActive != aBool;
}


FString UCesiumAsset::ByteSizeStringToGb(FString aByteSize)
{
	// Convert the FString to double
	double byteSize = FCString::Atof(*aByteSize);

	// Calculate gigabytes by dividing by 1024^3 (1,073,741,824)
	double gigabytes = byteSize / (1024.0 * 1024.0 * 1024.0);

	// Convert the result back to FString
	return FString::Printf(TEXT("%.2f GB"), gigabytes); // Format to 2 decimal places
}

void UCesiumAsset::ToggleCurrentlyActive() { fCurrentlyActive = !fCurrentlyActive; }

FString UCesiumAsset::GetId() { return fId; }
FString UCesiumAsset::GetItemName() { return (fCurrentlyActive ? TEXT("ACTIVE") : TEXT("")) + fDisplayName; }
FString UCesiumAsset::GetDisplayName() { return fDisplayName; }
bool UCesiumAsset::GetCurrentlyActive() { return fCurrentlyActive; }
FString UCesiumAsset::GetUploadDate()
{
	/*
	 returns a reformatted date of upload for UI display, replaces format:
	 From:
		YYYY-MM-DDTHH:MM:SS:MLSZ

	 To:
		YYYY/MM/DD
	*/
	FString returnValue = fUploadDate.Replace(TEXT("-"), TEXT("/"), ESearchCase::IgnoreCase);
	int32 firstTimeCharacter = returnValue.Find(TEXT("T"), ESearchCase::IgnoreCase);
	int32 lastTimeCharacter = returnValue.Len();
	returnValue.RemoveAt(firstTimeCharacter, lastTimeCharacter, true);
	return returnValue;
}
FString UCesiumAsset::GetDataType() { return fDataType; }
FString UCesiumAsset::GetDataSize() { return fDataSize; }

void UCesiumAsset::SetId(FString aValue) { fId = aValue; }
void UCesiumAsset::SetDisplayName(FString aValue) { fDisplayName = aValue; }
void UCesiumAsset::SetCurrentlyActive(bool aValue) { fCurrentlyActive = aValue; }
void UCesiumAsset::SetUploadDate(FString aValue) { fUploadDate = aValue; }
void UCesiumAsset::SetDataType(FString aValue) { fDataType = aValue; }
void UCesiumAsset::SetDataSize(FString aValue) { fDataSize = aValue; }

void UCesiumAsset::BeginDestroy() {
	UE_LOG(LogTemp, Error, TEXT("CesiumAsset being garbage collected, ID: %s"), *fId);

	Super::BeginDestroy();
}

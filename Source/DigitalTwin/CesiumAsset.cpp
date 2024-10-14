// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumAsset.h"

UCesiumAsset::UCesiumAsset()
{
	fId = "";
	fActualItemName = "";
	fDisplayName = "";
	fCurrentlyActive = false;
	fUploadDate = "";
	fDataType = "";
	fDataSize = "";
}

void UCesiumAsset::Construct(FString aId, FString aItemName, FString aDate, FString aDataType, FString aDataSize)
{
	fId = aId;
	fActualItemName = aItemName;
	fDisplayName = aItemName.Replace(TEXT("active"), TEXT(""), ESearchCase::IgnoreCase);
	fCurrentlyActive = aItemName.Contains(TEXT("active"), ESearchCase::IgnoreCase);
	fUploadDate = aDate;
	fDataType = aDataType;
	fDataSize = aDataSize;
}

bool UCesiumAsset::IsActiveDifferent(bool aBool)
{
	if (fCurrentlyActive != aBool)
	{
		// Update the stored item name accordingly if its active status has changed - this will then be renamed in cesium in a following step.
		fActualItemName = aBool ? ("ACTIVE" + fActualItemName) : (fActualItemName.Replace(TEXT("ACTIVE"), TEXT(""), ESearchCase::IgnoreCase));
	}
	// Return the status of an active/inactive state change request.
	return (fCurrentlyActive != aBool);
}

FString UCesiumAsset::GetId() { return fId; }
FString UCesiumAsset::GetItemName() { return fActualItemName; }
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

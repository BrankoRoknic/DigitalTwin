// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumAsset.h"

UCesiumAsset::UCesiumAsset() { }

void UCesiumAsset::Construct(FString aId, FString aItemName, FString aDate, FString aDataType)
{
	fId = aId;
	fDisplayName = aItemName.Replace(TEXT("active"), TEXT(""), ESearchCase::IgnoreCase);
	fCurrentlyActive = aItemName.Contains(TEXT("active"), ESearchCase::IgnoreCase);
	fUploadDate = aDate;
	fDataType = aDataType;
}

FString UCesiumAsset::GetId() { return fId; }
FString UCesiumAsset::GetItemName() { return (fCurrentlyActive ? "active" : "") + fDisplayName; }
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
	UE_LOG(LogTemp, Log, TEXT("The location of Character \"T\" is: %d"), returnValue.Find(TEXT("T")));
	int32 lCharsToRemove = returnValue.Find(TEXT("T"), ESearchCase::IgnoreCase) - returnValue.Len();
	int32 firstTimeCharacter = returnValue.Find(TEXT("T"), ESearchCase::IgnoreCase);
	int32 lastTimeCharacter = returnValue.Len();

	//returnValue.RemoveAt(returnValue.Find(TEXT("T")), returnValue.Find(TEXT("T")) - returnValue.Len(), true);
	returnValue.RemoveAt(firstTimeCharacter, lastTimeCharacter, true);
	return returnValue;
}
FString UCesiumAsset::GetDataType() { return fDataType; }

void UCesiumAsset::SetId(FString aValue) { fId = aValue; }
void UCesiumAsset::SetDisplayName(FString aValue) { fDisplayName = aValue; }
void UCesiumAsset::SetCurrentlyActive(bool aValue) { fCurrentlyActive = aValue; }
void UCesiumAsset::SetUploadDate(FString aValue) { fUploadDate = aValue; }
void UCesiumAsset::SetDataType(FString aValue) { fDataType = aValue; }

// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumAsset.h"

UCesiumAsset::UCesiumAsset()
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::UCesiumAsset: Constructor called."));

	fId = "";
	fDisplayName = "";
	fCurrentlyActive = false;
	fUploadDate = "";
	fDataType = "";
	fDataSize = "";

	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::UCesiumAsset: Object initialized with default values."));
}

void UCesiumAsset::Construct(FString aId, FString aItemName, FString aDate, FString aDataType, FString aDataSize)
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::Construct: Method called with parameters - aId: %s, aItemName: %s, aDate: %s, aDataType: %s, aDataSize: %s"), *aId, *aItemName, *aDate, *aDataType, *aDataSize);

	fId = aId;
	fDisplayName = aItemName.Replace(TEXT("active"), TEXT(""), ESearchCase::IgnoreCase);
	fCurrentlyActive = aItemName.Contains(TEXT("active"), ESearchCase::IgnoreCase);
	fUploadDate = aDate;
	fDataType = aDataType;
	fDataSize = aDataSize;

	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::Construct: Method completed. DisplayName: %s, CurrentlyActive: %s"), *fDisplayName, fCurrentlyActive ? TEXT("true") : TEXT("false"));
}


bool UCesiumAsset::IsActiveDifferent(bool aBool)
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::IsActiveDifferent: Method called with parameter - aBool: %s"), aBool ? TEXT("true") : TEXT("false"));

	bool result = fCurrentlyActive != aBool;

	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::IsActiveDifferent: Result - %s"), result ? TEXT("true") : TEXT("false"));
	return result;
}

void UCesiumAsset::ToggleCurrentlyActive()
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::ToggleCurrentlyActive: Method called."));
	fCurrentlyActive = !fCurrentlyActive;
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::ToggleCurrentlyActive: CurrentlyActive status is now - %s"), fCurrentlyActive ? TEXT("true") : TEXT("false"));
}

FString UCesiumAsset::GetId() {
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetId: Method called. ID: %s"), *fId);

	return fId;
}

FString UCesiumAsset::GetItemName() 
{
	FString result = (fCurrentlyActive ? TEXT("ACTIVE") : TEXT("")) + fDisplayName;

	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetItemName: Method called. Result: %s"), *result);

	return result;

}

FString UCesiumAsset::GetDisplayName() {
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetDisplayName: Method called. DisplayName: %s"), *fDisplayName);

	return fDisplayName;
}

bool UCesiumAsset::GetCurrentlyActive() {
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetCurrentlyActive: Method called. CurrentlyActive: %s"), fCurrentlyActive ? TEXT("true") : TEXT("false"));

	return fCurrentlyActive;
}

FString UCesiumAsset::GetUploadDate()
{
	/*
	 returns a reformatted date of upload for UI display, replaces format:
	 From:
		YYYY-MM-DDTHH:MM:SS:MLSZ

	 To:
		YYYY/MM/DD
	*/
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetUploadDate: Method called. Original Date: %s"), *fUploadDate);

	FString returnValue = fUploadDate.Replace(TEXT("-"), TEXT("/"), ESearchCase::IgnoreCase);
	int32 firstTimeCharacter = returnValue.Find(TEXT("T"), ESearchCase::IgnoreCase);
	int32 lastTimeCharacter = returnValue.Len();
	returnValue.RemoveAt(firstTimeCharacter, lastTimeCharacter, true);

	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetUploadDate: Formatted result: %s"), *returnValue);

	return returnValue;
}

FString UCesiumAsset::GetDataType() { 
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetDataType: Method called. DataType: %s"), *fDataType);
	return fDataType;
}

FString UCesiumAsset::GetDataSize() { 
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::GetDataSize: Method called. DataSize: %s"), *fDataSize);
	return fDataSize;
}

void UCesiumAsset::SetId(FString aValue) { 
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetId: Method called. New ID: %s"), *aValue);
	fId = aValue;
}

void UCesiumAsset::SetDisplayName(FString aValue) 
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetDisplayName: Method called. New DisplayName: %s"), *aValue);

	fDisplayName = aValue;
}


void UCesiumAsset::SetCurrentlyActive(bool aValue) 
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetCurrentlyActive: Method called. New CurrentlyActive status: %s"), aValue ? TEXT("true") : TEXT("false"));

	fCurrentlyActive = aValue;
}

void UCesiumAsset::SetUploadDate(FString aValue) 
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetUploadDate: Method called. New UploadDate: %s"), *aValue);

	fUploadDate = aValue;
}

void UCesiumAsset::SetDataType(FString aValue) 
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetDataType: Method called. New DataType: %s"), *aValue);

	fDataType = aValue;
}

void UCesiumAsset::SetDataSize(FString aValue) 
{
	UE_LOG(LogTemp, Log, TEXT("INFO: UCesiumAsset::SetDataSize: Method called. New DataSize: %s"), *aValue);

	fDataSize = aValue;
}


void UCesiumAsset::BeginDestroy() {
	UE_LOG(LogTemp, Error, TEXT("ERROR: UCesiumAsset::BeginDestroy: UCesiumAsset being destroyed. Asset ID: %s"), *fId);

	Super::BeginDestroy();
}


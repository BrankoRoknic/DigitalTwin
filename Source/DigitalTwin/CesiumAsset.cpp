// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumAsset.h"

UCesiumAsset::UCesiumAsset()
{
	// LOG_CODE_4001: UCesiumAsset constructor called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4001: UCesiumAsset constructor called."));

	fId = "";
	fDisplayName = "";
	fCurrentlyActive = false;
	fUploadDate = "";
	fDataType = "";
	fDataSize = "";

 // LOG_CODE_4002: UCesiumAsset object initialized
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4002: UCesiumAsset object initialized with default values."));
}

void UCesiumAsset::Construct(FString aId, FString aItemName, FString aDate, FString aDataType, FString aDataSize)
{
	// LOG_CODE_4003: Construct method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4003: Construct method called with parameters - aId: %s, aItemName: %s, aDate: %s, aDataType: %s, aDataSize: %s"), *aId, *aItemName, *aDate, *aDataType, *aDataSize);

    fId = aId;
    fDisplayName = aItemName.Replace(TEXT("active"), TEXT(""), ESearchCase::IgnoreCase);
    fCurrentlyActive = aItemName.Contains(TEXT("active"), ESearchCase::IgnoreCase);
    fUploadDate = aDate;
    fDataType = aDataType;
    fDataSize = aDataSize;

    // LOG_CODE_4004: Construct method completed
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4004: Construct method completed. DisplayName: %s, CurrentlyActive: %s"), *fDisplayName, fCurrentlyActive ? TEXT("true") : TEXT("false"));
}


bool UCesiumAsset::IsActiveDifferent(bool aBool)
{
	 // LOG_CODE_4005: IsActiveDifferent method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4005: IsActiveDifferent method called with parameter - aBool: %s"), aBool ? TEXT("true") : TEXT("false"));

    bool result = fCurrentlyActive != aBool;

    // LOG_CODE_4006: IsActiveDifferent result
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4006: IsActiveDifferent result - %s"), result ? TEXT("true") : TEXT("false"));
    return result;
}

void UCesiumAsset::ToggleCurrentlyActive()
{
	 UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4007: ToggleCurrentlyActive method called."));

    fCurrentlyActive = !fCurrentlyActive;

    // LOG_CODE_4008: CurrentlyActive status toggled
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4008: CurrentlyActive status is now - %s"), fCurrentlyActive ? TEXT("true") : TEXT("false"));
}

FString UCesiumAsset::GetId() {
    // LOG_CODE_4009: GetId method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4009: GetId method called. ID: %s"), *fId);

    return fId;
}
FString UCesiumAsset::GetItemName() 
{
	 return (fCurrentlyActive ? TEXT("ACTIVE") : TEXT("")) + fDisplayName; 
	 // LOG_CODE_4010: GetItemName method called, resulting in
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4010: GetItemName method called."));

}
FString UCesiumAsset::GetDisplayName() {
	// LOG_CODE_4011: GetDisplayName method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4011: GetDisplayName method called. DisplayName: %s"), *fDisplayName);

	 return fDisplayName; 
}
bool UCesiumAsset::GetCurrentlyActive() {
	 // LOG_CODE_4012: GetCurrentlyActive method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4012: GetCurrentlyActive method called. CurrentlyActive: %s"), fCurrentlyActive ? TEXT("true") : TEXT("false"));
 return fCurrentlyActive; }
FString UCesiumAsset::GetUploadDate()
{
	/*
	 returns a reformatted date of upload for UI display, replaces format:
	 From:
		YYYY-MM-DDTHH:MM:SS:MLSZ

	 To:
		YYYY/MM/DD
	*/
// LOG_CODE_4013: GetUploadDate method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4013: GetUploadDate method called. Original Date: %s"), *fUploadDate);

	FString returnValue = fUploadDate.Replace(TEXT("-"), TEXT("/"), ESearchCase::IgnoreCase);
	int32 firstTimeCharacter = returnValue.Find(TEXT("T"), ESearchCase::IgnoreCase);
	int32 lastTimeCharacter = returnValue.Len();
	returnValue.RemoveAt(firstTimeCharacter, lastTimeCharacter, true);
	// LOG_CODE_4014: GetUploadDate formatted result
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4014: GetUploadDate formatted result: %s"), *returnValue);

	return returnValue;
}
FString UCesiumAsset::GetDataType() { 
	 // LOG_CODE_4015: GetDataType method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4015: GetDataType method called. DataType: %s"), *fDataType);
	return fDataType; }
FString UCesiumAsset::GetDataSize() { 
	// LOG_CODE_4016: GetDataSize method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4016: GetDataSize method called. DataSize: %s"), *fDataSize);
	return fDataSize; }

void UCesiumAsset::SetId(FString aValue) { 
	  // LOG_CODE_4017: SetId method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4017: SetId method called. New ID: %s"), *aValue);
	fId = aValue; }
void UCesiumAsset::SetDisplayName(FString aValue) 
{
    // LOG_CODE_4018: SetDisplayName method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4018: SetDisplayName method called. New DisplayName: %s"), *aValue);

    fDisplayName = aValue;
}

void UCesiumAsset::SetCurrentlyActive(bool aValue) 
{
    // LOG_CODE_4019: SetCurrentlyActive method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4019: SetCurrentlyActive method called. New CurrentlyActive status: %s"), aValue ? TEXT("true") : TEXT("false"));

    fCurrentlyActive = aValue;
}

void UCesiumAsset::SetUploadDate(FString aValue) 
{
    // LOG_CODE_4020: SetUploadDate method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4020: SetUploadDate method called. New UploadDate: %s"), *aValue);

    fUploadDate = aValue;
}

void UCesiumAsset::SetDataType(FString aValue) 
{
    // LOG_CODE_4021: SetDataType method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4021: SetDataType method called. New DataType: %s"), *aValue);

    fDataType = aValue;
}

void UCesiumAsset::SetDataSize(FString aValue) 
{
    // LOG_CODE_4022: SetDataSize method called
    UE_LOG(LogTemp, Log, TEXT("LOG_CODE_4022: SetDataSize method called. New DataSize: %s"), *aValue);

    fDataSize = aValue;
}

void UCesiumAsset::BeginDestroy() {
	UE_LOG(LogTemp, Error, TEXT("Death star activating: %s"), *fId);
	 // LOG_CODE_4023: BeginDestroy method called
    UE_LOG(LogTemp, Error, TEXT("LOG_CODE_4023: UCesiumAsset being destroyed. Asset ID: %s"), *fId);

	Super::BeginDestroy();
}

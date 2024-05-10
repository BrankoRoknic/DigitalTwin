// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"

/**
 * 
 */
class DIGITALTWIN_API cesiumUploader
{
public:
	cesiumUploader();
	~cesiumUploader();

	void UploadLasFileToCesium(const FString& FilePath, const FString& AccessToken);


private:
	// Helper functions to handle HTTP responses
	void OnAssetCreationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UploadFileToCesium(const FString& UploadLocation, const TArray<uint8>& FileData, const FString& AssetId);
	void OnFileUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString AssetId);

	// File data to upload
	TArray<uint8> FileData;
	
};

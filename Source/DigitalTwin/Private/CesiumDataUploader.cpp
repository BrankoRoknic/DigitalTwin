// Fill out your copyright notice in the Description page of Project Settings.

// CesiumIonUploader.cpp
#include "CesiumDataUploader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

UCesiumDataUploader::UCesiumDataUploader()
{
	// Constructor code, if any specific initialization is required
}
void UCesiumDataUploader::UploadToCesiumIon(const FString& FilePath, const FString& AccessToken)
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindStatic(&UCesiumDataUploader::OnUploadComplete);
	Request->SetURL(TEXT("https://api.cesium.com/v1/assets"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + AccessToken);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));

	// Load the file data
	TArray<uint8> FileData;
	if (FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		Request->SetContent(FileData);
		// Send the request
		Request->ProcessRequest();
	} 
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
	}
}

void UCesiumDataUploader::OnUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response)
	{
		UE_LOG(LogTemp, Error, TEXT("HTTP Request Failed or No Response was received"));
		return;
	}

	if (Response->GetResponseCode() == 200)
	{
		UE_LOG(LogTemp, Log, TEXT("Upload Successful: %s"), *Response->GetContentAsString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Upload Failed: %s. HTTP Status Code: %d"), *Response->GetContentAsString(),
		       Response->GetResponseCode());
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

// CesiumIonUploader.cpp
#include "CesiumDataUploader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"

UCesiumDataUploader::UCesiumDataUploader()
{
	// Constructor code, if any specific initialization is required
}

void UCesiumDataUploader::UploadToCesiumIon(const FString& FilePath, const FString& AccessToken)
{
    FString FileExtension = FPaths::GetExtension(FilePath).ToLower();
    FString FileType;
    FString SourceType;

    // Map file extensions to their respective types and source types
    if (FileExtension == TEXT("las") || FileExtension == TEXT("laz"))
    {
        FileType = TEXT("3DTILES");
        SourceType = TEXT("POINT_CLOUD");
    }
    else if (FileExtension == TEXT("obj"))
    {
        FileType = TEXT("3DTILES");
        SourceType = TEXT("3D_MODEL");
    }
    else if (FileExtension == TEXT("kmz"))
    {
        FileType = TEXT("KML");
        SourceType = TEXT("KML");
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Unsupported file type: %s"), *FileExtension);
        return;
    }

    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindStatic(&UCesiumDataUploader::OnUploadComplete);
    Request->SetURL(TEXT("https://api.cesium.com/v1/assets"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + AccessToken);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    FString JsonPayload = FString::Printf(TEXT("{\"name\": \"%s\", \"description\": \"Uploaded via Unreal Engine\", \"type\": \"%s\", \"options\": {\"sourceType\": \"%s\"}}"), *FPaths::GetCleanFilename(FilePath), *FileType, *SourceType);
    Request->SetContentAsString(JsonPayload);

    Request->ProcessRequest();
}

void UCesiumDataUploader::OnUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Upload failed. No response or request was unsuccessful."));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode >= 200 && ResponseCode < 300)
    {
        UE_LOG(LogTemp, Log, TEXT("Upload succeeded: %s"), *ResponseContent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Upload Failed: %s. HTTP Status Code: %d"), *ResponseContent, ResponseCode);
    }
}

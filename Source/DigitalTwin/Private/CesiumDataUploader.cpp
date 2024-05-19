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

    // Map file extensions to their respective types
    if (FileExtension == TEXT("las")) FileType = TEXT("LAS");
    else if (FileExtension == TEXT("laz")) FileType = TEXT("LAZ");
    else if (FileExtension == TEXT("obj")) FileType = TEXT("3DTILES");
    else if (FileExtension == TEXT("kmz")) FileType = TEXT("KMZ");
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
    Request->SetHeader(TEXT("Content-Type"), TEXT("multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW"));

    TArray<uint8> FileData;
    if (FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        FString Boundary = TEXT("----WebKitFormBoundary7MA4YWxkTrZu0gW");
        FString BeginBoundary = TEXT("--") + Boundary + TEXT("\r\n");
        FString EndBoundary = TEXT("--") + Boundary + TEXT("--\r\n");

        FString BodyData;
        BodyData += BeginBoundary;
        BodyData += TEXT("Content-Disposition: form-data; name=\"type\"\r\n\r\n");
        BodyData += FileType + TEXT("\r\n");
        BodyData += BeginBoundary;
        BodyData += TEXT("Content-Disposition: form-data; name=\"file\"; filename=\"") + FPaths::GetCleanFilename(FilePath) + TEXT("\"\r\n");
        BodyData += TEXT("Content-Type: application/octet-stream\r\n\r\n");

        TArray<uint8> RequestContent;
        RequestContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*BodyData)), BodyData.Len());
        RequestContent.Append(FileData);
        RequestContent.Append(reinterpret_cast<const uint8*>(TCHAR_TO_UTF8(*EndBoundary)), EndBoundary.Len());

        Request->SetContent(RequestContent);

        Request->ProcessRequest();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
    }
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

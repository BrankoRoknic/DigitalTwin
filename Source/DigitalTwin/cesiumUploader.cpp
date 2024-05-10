// Fill out your copyright notice in the Description page of Project Settings.
#include "cesiumUploader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"


cesiumUploader::cesiumUploader(){}

cesiumUploader::~cesiumUploader() {}

	// Read the file
    void cesiumUploader::UploadLasFileToCesium(const FString& FilePath, const FString& AccessToken) {
        // Read the file into the member variable `UploadedFileData`
        if (!FFileHelper::LoadFileToArray(this->FileData, *FilePath)) {
            UE_LOG(LogTemp, Error, TEXT("Failed to read .las file at %s"), *FilePath);
            return;
        }

        // Create the HTTP request to create the asset on Cesium ion
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
        HttpRequest->SetURL(TEXT("https://api.cesium.com/v1/assets"));
        HttpRequest->SetVerb(TEXT("POST"));
        HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        HttpRequest->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));

        // Prepare the JSON payload for asset creation
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
        JsonObject->SetStringField(TEXT("name"), TEXT("LiDAR Data"));
        JsonObject->SetStringField(TEXT("description"), TEXT("LiDAR data uploaded from Unreal Engine"));
        JsonObject->SetStringField(TEXT("type"), TEXT("3DTiles"));

        FString JsonString;
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
        FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

        HttpRequest->SetContentAsString(JsonString);
        HttpRequest->OnProcessRequestComplete().BindRaw(this, &cesiumUploader::OnAssetCreationResponse);
        HttpRequest->ProcessRequest();
    }

void cesiumUploader::OnAssetCreationResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200) {
        UE_LOG(LogTemp, Error, TEXT("Asset creation request failed"));
        return;
    }

    // Parse the response to get the upload location
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) {
        FString UploadLocation = JsonObject->GetStringField(TEXT("uploadLocation"));
        FString AssetId = JsonObject->GetStringField(TEXT("id"));

        // Upload the file data to Cesium ion
        UploadFileToCesium(UploadLocation, FileData, AssetId);
    }
}

void cesiumUploader::UploadFileToCesium(const FString& UploadLocation, const TArray<uint8>& FileData, const FString& AssetId) {
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(UploadLocation);
    HttpRequest->SetVerb(TEXT("PUT"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    HttpRequest->SetContent(FileData);

    HttpRequest->OnProcessRequestComplete().BindRaw(this, &cesiumUploader::OnFileUploadResponse, AssetId);
    HttpRequest->ProcessRequest();
}

void cesiumUploader::OnFileUploadResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString AssetId) {
    if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200) {
        UE_LOG(LogTemp, Error, TEXT("File upload request failed"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully uploaded .las file to Cesium ion with Asset ID: %s"), *AssetId);
}
	
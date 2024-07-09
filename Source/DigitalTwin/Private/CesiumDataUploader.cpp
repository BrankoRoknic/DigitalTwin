#include "CesiumDataUploader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

UCesiumDataUploader::UCesiumDataUploader()
{
    // Constructor code if needed
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

    CurrentFilePath = FilePath;
    CurrentAccessToken = AccessToken;

    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UCesiumDataUploader::OnCreateAssetMetadataComplete);
    Request->SetURL(TEXT("https://api.cesium.com/v1/assets"));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + AccessToken);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    FString JsonPayload = FString::Printf(TEXT("{\"name\": \"%s\", \"description\": \"Uploaded via Unreal Engine\", \"type\": \"%s\", \"options\": {\"sourceType\": \"%s\"}}"), *FPaths::GetCleanFilename(FilePath), *FileType, *SourceType);
    Request->SetContentAsString(JsonPayload);

    Request->ProcessRequest();
}

void UCesiumDataUploader::OnCreateAssetMetadataComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Asset metadata creation failed."));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode >= 200 && ResponseCode < 300)
    {
        UE_LOG(LogTemp, Log, TEXT("Asset metadata created: %s"), *ResponseContent);

        // Parse the JSON response to get upload details
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
        if (FJsonSerializer::Deserialize(Reader, JsonObject))
        {
            FString Endpoint = JsonObject->GetStringField("uploadLocation.endpoint");
            FString Bucket = JsonObject->GetStringField("uploadLocation.bucket");
            FString Prefix = JsonObject->GetStringField("uploadLocation.prefix");
            FString AccessKey = JsonObject->GetStringField("uploadLocation.accessKey");
            FString SecretAccessKey = JsonObject->GetStringField("uploadLocation.secretAccessKey");
            FString SessionToken = JsonObject->GetStringField("uploadLocation.sessionToken");
            CurrentAssetId = JsonObject->GetObjectField("assetMetadata")->GetStringField("id");

            // Proceed to upload the file to S3
            UploadFileToS3(CurrentFilePath, Endpoint, Bucket, Prefix, AccessKey, SecretAccessKey, SessionToken);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Asset metadata creation failed: %s. HTTP Status Code: %d"), *ResponseContent, ResponseCode);
    }
}

void UCesiumDataUploader::UploadFileToS3(const FString& FilePath, const FString& Endpoint, const FString& Bucket, const FString& Prefix, const FString& AccessKey, const FString& SecretAccessKey, const FString& SessionToken)
{
    // Load the file data
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return;
    }

    FString Url = FString::Printf(TEXT("%s/%s/%s%s"), *Endpoint, *Bucket, *Prefix, *FPaths::GetCleanFilename(FilePath));
    
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(TEXT("PUT"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    Request->SetHeader(TEXT("x-amz-security-token"), SessionToken);
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("AWS4-HMAC-SHA256 Credential=%s/20230520/us-east-1/s3/aws4_request, SignedHeaders=host;x-amz-acl;x-amz-content-sha256;x-amz-date, Signature=%s"), *AccessKey, *SecretAccessKey));
    Request->SetContent(FileData);

    Request->OnProcessRequestComplete().BindUObject(this, &UCesiumDataUploader::OnS3UploadComplete);
    Request->ProcessRequest();
}

void UCesiumDataUploader::OnS3UploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Upload to S3 failed."));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode >= 200 && ResponseCode < 300)
    {
        UE_LOG(LogTemp, Log, TEXT("S3 upload succeeded: %s"), *ResponseContent);

        // Notify Cesium that the upload is complete
        NotifyUploadComplete();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("S3 upload failed: %s. HTTP Status Code: %d"), *ResponseContent, ResponseCode);
    }
}

void UCesiumDataUploader::NotifyUploadComplete()
{
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &UCesiumDataUploader::OnUploadComplete);
    Request->SetURL(FString::Printf(TEXT("https://api.cesium.com/v1/assets/%s/uploadComplete"), *CurrentAssetId));
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Authorization"), FString(TEXT("Bearer ")) + CurrentAccessToken);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    Request->ProcessRequest();
}

void UCesiumDataUploader::OnUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Upload completion notification failed."));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    FString ResponseContent = Response->GetContentAsString();

    if (ResponseCode >= 200 && ResponseCode < 300)
    {
        UE_LOG(LogTemp, Log, TEXT("Upload completed successfully: %s"), *ResponseContent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Upload completion failed: %s. HTTP Status Code: %d"), *ResponseContent, ResponseCode);
    }
}

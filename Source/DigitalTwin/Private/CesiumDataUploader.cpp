#ifdef CESIUMDATAUPLOADER_EXPORTS
#define CESIUMDATAUPLOADER_API __declspec(dllexport)
#else
#define CESIUMDATAUPLOADER_API __declspec(dllimport)
#endif
#include "CesiumDataUploader.h"


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
        UE_LOG(LogTemp, Error, TEXT("Asset metadata creation failed due to an invalid response or a network error."));
        return;
    }

    int32 ResponseCode = Response->GetResponseCode();
    if (ResponseCode < 200 || ResponseCode >= 300)
    {
        FString ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Error, TEXT("Asset metadata creation failed with HTTP Status Code: %d and response: %s"), ResponseCode, *ResponseContent);
        return;
    }

    FString ResponseContent = Response->GetContentAsString();
    UE_LOG(LogTemp, Log, TEXT("Asset metadata created: %s"), *ResponseContent);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseContent);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON response."));
        return;
    }
  
    const TSharedPtr<FJsonObject>* UploadLocationObject;
    if (!JsonObject->TryGetObjectField(TEXT("uploadLocation"), UploadLocationObject))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to locate 'uploadLocation' in JSON response."));
        return;
    }

    FString Endpoint = (*UploadLocationObject)->GetStringField(TEXT("endpoint"));
    FString Bucket = (*UploadLocationObject)->GetStringField(TEXT("bucket"));
    FString Prefix = (*UploadLocationObject)->GetStringField(TEXT("prefix"));
    FString AccessKey = (*UploadLocationObject)->GetStringField(TEXT("accessKey"));
    FString SecretAccessKey = (*UploadLocationObject)->GetStringField(TEXT("secretAccessKey"));
    FString SessionToken = (*UploadLocationObject)->GetStringField(TEXT("sessionToken"));
    FString AssetId = JsonObject->GetObjectField(TEXT("assetMetadata"))->GetStringField(TEXT("id"));
    

    UploadFileToS3(CurrentFilePath, Endpoint, Bucket, Prefix, AccessKey, SecretAccessKey, SessionToken);
}


void UCesiumDataUploader::UploadFileToS3(const FString& FilePath, const FString& Endpoint, const FString& Bucket, const FString& Prefix, const FString& AccessKey, const FString& SecretAccessKey, const FString& SessionToken)
{
    UE_LOG(LogTemp, Log, TEXT("endpoint: %s"), *Endpoint);
    UE_LOG(LogTemp, Log, TEXT("Bucket: %s"), *Bucket);
    UE_LOG(LogTemp, Log, TEXT("AccessKey: %s"), *AccessKey);
    UE_LOG(LogTemp, Log, TEXT("SecretAccessKey: %s"), *SecretAccessKey);
    
    TArray<uint8> FileData;
    if (!FFileHelper::LoadFileToArray(FileData, *FilePath)) {
        UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
        return;
    }

    Aws::SDKOptions options;
    Aws::InitAPI(options);

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.region = Aws::Region::US_EAST_1;
    clientConfig.endpointOverride = TCHAR_TO_UTF8(*Endpoint);

    Aws::Auth::AWSCredentials credentials(TCHAR_TO_UTF8(*AccessKey), TCHAR_TO_UTF8(*SecretAccessKey), TCHAR_TO_UTF8(*SessionToken));

    Aws::S3::S3Client s3_client(credentials, NULL,clientConfig);

    Aws::S3::Model::PutObjectRequest object_request;
    object_request.WithBucket(TCHAR_TO_UTF8(*Bucket))
                  .WithKey(TCHAR_TO_UTF8(*FPaths::Combine(Prefix, FPaths::GetCleanFilename(FilePath))))
                  .SetContentType("application/octet-stream");

    auto DataStream = Aws::MakeShared<Aws::StringStream>("");
    DataStream->write(reinterpret_cast<char*>(FileData.GetData()), FileData.Num());
    object_request.SetBody(DataStream);

    auto put_object_outcome = s3_client.PutObject(object_request);

    if (put_object_outcome.IsSuccess()) {
        UE_LOG(LogTemp, Log, TEXT("Successfully uploaded to S3"));
    } else {
        UE_LOG(LogTemp, Error, TEXT("Failed to upload to S3: %hs"), UTF8_TO_TCHAR(put_object_outcome.GetError().GetMessage().c_str()));
    }

    Aws::ShutdownAPI(options);
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

    #include "CesiumDataUploader.h"
    #include "HttpModule.h"
    #include "Interfaces/IHttpRequest.h"
    #include "Interfaces/IHttpResponse.h"
    #include "Misc/FileHelper.h"
    #include "Dom/JsonObject.h"
    #include "Serialization/JsonSerializer.h"
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>>
UCesiumDataUploader::UCesiumDataUploader()
: CurrentFilePath(TEXT("")), CurrentAccessToken(TEXT("")), CurrentAssetId(TEXT("")) // Initialize member variables
{
    // Initialize the AWS SDK
    Aws::SDKOptions options;
    Aws::InitAPI(options);
}

UCesiumDataUploader::~UCesiumDataUploader()
{
    // Shutdown the AWS SDK
    Aws::SDKOptions options;
    Aws::ShutdownAPI(options);
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
                TSharedPtr<FJsonObject> UploadLocation = JsonObject->GetObjectField("uploadLocation");
                FString Endpoint = UploadLocation->GetStringField("endpoint");
                FString Bucket = UploadLocation->GetStringField("bucket");
                FString Prefix = UploadLocation->GetStringField("prefix");
                FString AccessKey = UploadLocation->GetStringField("accessKey");
                FString SecretAccessKey = UploadLocation->GetStringField("secretAccessKey");
                FString SessionToken = UploadLocation->GetStringField("sessionToken");
                CurrentAssetId = JsonObject->GetStringField("id");

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

    Aws::Client::ClientConfiguration ClientConfig;
    ClientConfig.endpointOverride = TCHAR_TO_UTF8(*Endpoint);

    auto CredentialsProvider = Aws::MakeShared<Aws::Auth::SimpleAWSCredentialsProvider>(
        "S3ClientProvider",
        TCHAR_TO_UTF8(*AccessKey),
        TCHAR_TO_UTF8(*SecretAccessKey),
        TCHAR_TO_UTF8(*SessionToken)
    );

    Aws::S3::S3Client S3Client(CredentialsProvider, ClientConfig);

    Aws::S3::Model::PutObjectRequest ObjectRequest;
    ObjectRequest.SetBucket(TCHAR_TO_UTF8(*Bucket));
    ObjectRequest.SetKey(TCHAR_TO_UTF8(*(Prefix + FPaths::GetCleanFilename(FilePath))));

    auto DataStream = Aws::MakeShared<Aws::StringStream>("");
    DataStream->write(reinterpret_cast<char*>(FileData.GetData()), FileData.Num());
    ObjectRequest.SetBody(DataStream);
    ObjectRequest.SetContentType("application/octet-stream");

    auto PutObjectOutcome = S3Client.PutObject(ObjectRequest);

    if (PutObjectOutcome.IsSuccess())
    {
        UE_LOG(LogTemp, Log, TEXT("S3 upload succeeded"));
        NotifyUploadComplete();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("S3 upload failed: %s"), *FString(PutObjectOutcome.GetError().GetMessage().c_str()));
    }
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

#pragma once
#ifdef CESIUMDATAUPLOADER_EXPORTS
#define CESIUMDATAUPLOADER_API __declspec(dllexport)
#else
#define CESIUMDATAUPLOADER_API __declspec(dllimport)
#endif
#include "AWSSDK/Include/aws/core/Aws.h"
#include "AWSSDK/Include/aws/core/auth/AWSCredentials.h"
#include "AWSSDK/Include/aws/s3/S3Client.h"
#include "AWSSDK/Include/aws/s3/model/PutObjectRequest.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "CoreMinimal.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/NoExportTypes.h"
#include "CesiumDataUploader.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumDataUploader : public UObject
{
	GENERATED_BODY()

public:
	UCesiumDataUploader();
	//~UCesiumDataUploader();

	UFUNCTION(BlueprintCallable, Category = "Cesium Ion")
	void UploadToCesiumIon(const FString& FilePath, const FString& AccessToken);

private:
	FString CurrentFilePath;
	FString CurrentAccessToken;
	FString CurrentAssetId;

	void OnCreateAssetMetadataComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void UploadFileToS3(const FString& FilePath, const FString& Endpoint, const FString& Bucket, const FString& Prefix, const FString& AccessKey, const FString& SecretAccessKey, const FString& SessionToken);
	void NotifyUploadComplete();
	void OnS3UploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};

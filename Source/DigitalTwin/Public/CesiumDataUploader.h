#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "CesiumDataUploader.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumDataUploader : public UObject
{
	GENERATED_BODY()

public:
	UCesiumDataUploader();

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

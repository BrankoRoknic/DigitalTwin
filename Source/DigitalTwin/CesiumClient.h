#pragma once

#include "Cesium3DTileset.h"
#include "CesiumGeoreference.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Json.h"
#include "CoreMinimal.h"
#include <functional>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>
#include <string>
#include <cassert>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <filesystem>
#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/sha.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
THIRD_PARTY_INCLUDES_END
#undef UI
#include "CesiumClient.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumClient : public UObject
{
	GENERATED_BODY()
private:
	FString fCesiumToken;
	FString fFileName;
	FString fNotifyCompleteURL;
	FString fNotifyCompleteVerb;
	TArray<FString> fActiveAssets;
	int32 fFileSize;
public:
	UCesiumClient();
	UFUNCTION(BlueprintCallable, Category = "Upload")
	virtual void UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType);
	void ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	void OnS3UploadProgress(FHttpRequestPtr request, int32 bytesSent, int32 bytesReceived);
	void NotifyCesiumUploadComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	void OnCesiumUploadCompletion(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "List")
	void ListAssets(bool retreiveFlag);
	void ListResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Retrieve")
	void RetrieveActiveAssets();
	void StoreActiveAssets(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Render")
	void RenderAssetsInLevel();

	UFUNCTION(BlueprintCallable, Category = "GetfActiveAssets")
	TArray<FString> GetfActiveAssets();
};

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "Cesium3DTileset.h"
#include "CesiumGeoreference.h"
#include "CesiumAsset.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRetreiveAllAssetsResponse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCesiumUploadCompletionResponse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRetrieveActiveAssetsResponse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeleteAssetFromCesiumIonResponse);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateAssetActiveStateResponse);

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumClient : public UObject
{
	GENERATED_BODY()
private:
	FString fCesiumToken;
	FString fFileName;
	FString fNotifyCompleteURL;
	FString fNotifyCompleteVerb;
	double fSpaceUsed;
	TArray<FString> fIgnoredAssets;
	TArray<FString> fActiveLas;
	TArray<FString> fActiveTif;
	int32 fFileSize;
	FHttpModule* Http;
public:
	UPROPERTY()
	TArray<UCesiumAsset*> fAllAssetData;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FRetreiveAllAssetsResponse RetreiveAllAssetsResponse;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCesiumUploadCompletionResponse OnCesiumUploadCompletionResponse;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FRetrieveActiveAssetsResponse RetrieveActiveAssetsResponse;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FDeleteAssetFromCesiumIonResponse DeleteAssetFromCesiumIonResponse;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FUpdateAssetActiveStateResponse UpdateAssetActiveStateResponse;

	UCesiumClient();
	// Override BeginDestroy to handle cleanup
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Create")
	virtual void UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType);
	void ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	void OnS3UploadProgress(FHttpRequestPtr request, int32 bytesSent, int32 bytesReceived);
	void NotifyCesiumUploadComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	void OnCesiumUploadCompletion(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient List")
	void ListAssets(bool retreiveFlag);
	void ListResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Retrieve")
	void RetrieveActiveAssets();
	void StoreActiveAssets(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Retrieve")
	void RetrieveAllAssets();
	void StoreAllAssets(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Update")
	void UpdateAssetActiveState(UCesiumAsset* aCesiumAsset);
	void LogCesiumResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Delete")
	void DeleteAssetFromCesiumIon(UCesiumAsset* aCesiumAsset);
	void DeleteAssetResponse(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	TArray<FString> GetActiveTif();

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	TArray<UCesiumAsset*> GetAllAssetData();

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	UCesiumAsset* GetAllAssetDataElementByID(FString aId);

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	int32 GetAllAssetSize();

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	TArray<FString> GetActiveLas();

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	FString GetCesiumToken();

	UFUNCTION(BlueprintCallable, Category = "CesiumClient Get Methods")
	FString GetSpaceAvailableAsString();
	void AddToSpaceUsed(FString aSize);
};

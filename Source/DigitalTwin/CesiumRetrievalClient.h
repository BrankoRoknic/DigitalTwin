#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "CesiumRetrievalClient.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumRetrievalClient : public UObject
{
    GENERATED_BODY()

public:
    UCesiumRetrievalClient();

    UFUNCTION(BlueprintCallable, Category = "Cesium")
    void FetchAssets();

    UFUNCTION(BlueprintCallable, Category = "Cesium")
    void FetchAssetDetails(int32 AssetId);

    UFUNCTION(BlueprintCallable, Category = "Cesium")
    void LoadAsset(int32 AssetId);

    UFUNCTION(BlueprintCallable, Category = "Cesium")
    void ModifyAsset(int32 AssetId, const FString& NewName, const FString& NewDescription);

    UFUNCTION(BlueprintCallable, Category = "Cesium")
    void DeleteAsset(int32 AssetId);

private:
    FString CesiumAccessToken;

    void OnFetchAssetsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnFetchAssetDetailsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnLoadAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnModifyAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnDeleteAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void MakeHttpRequest(const FString& Url, const FString& Verb, const FString& Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> ResponseCallback);
};

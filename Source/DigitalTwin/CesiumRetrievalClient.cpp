#include "CesiumRetrievalClient.h"
#include "Json.h"
#include "JsonUtilities.h"

UCesiumRetrievalClient::UCesiumRetrievalClient()
{
    // Replace this with your hardcoded Cesium API token
    CesiumAccessToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiIwM2MzYTRlNC04MzMzLTRhMDktODVjZS00Mjc0NWRjNGYyNjAiLCJpZCI6MjEzODI0LCJpYXQiOjE3MjE5ODk4MjV9.aDuw8NxL3XgyrWkZ7oqmhX6ImPXJgUG8ZCnxu--UPDs";
    UE_LOG(LogTemp, Log, TEXT("CesiumRetrievalClient initialized with token: %s"), *CesiumAccessToken);
}

void UCesiumRetrievalClient::MakeHttpRequest(const FString& Url, const FString& Verb, const FString& Content, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> ResponseCallback)
{
    FHttpModule* Http = &FHttpModule::Get();
    if (!Http)
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP module not found"));
        return;
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(Verb);
    Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *CesiumAccessToken));
    Request->SetHeader("Content-Type", "application/json");

    if (!Content.IsEmpty())
    {
        Request->SetContentAsString(Content);
        UE_LOG(LogTemp, Log, TEXT("Request Content: %s"), *Content);
    }

    UE_LOG(LogTemp, Log, TEXT("Sending %s request to URL: %s"), *Verb, *Url);

    Request->OnProcessRequestComplete().BindLambda(ResponseCallback);
    Request->ProcessRequest();
}

void UCesiumRetrievalClient::FetchAssets()
{
    FString Url = "https://api.cesium.com/v1/assets";
    UE_LOG(LogTemp, Log, TEXT("Fetching assets from URL: %s"), *Url);
    MakeHttpRequest(Url, "GET", "", [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        OnFetchAssetsResponse(Request, Response, bWasSuccessful);
    });
}

void UCesiumRetrievalClient::FetchAssetDetails(int32 AssetId)
{
    FString Url = FString::Printf(TEXT("https://api.cesium.com/v1/assets/%d"), AssetId);
    UE_LOG(LogTemp, Log, TEXT("Fetching details for asset ID: %d from URL: %s"), AssetId, *Url);
    MakeHttpRequest(Url, "GET", "", [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        OnFetchAssetDetailsResponse(Request, Response, bWasSuccessful);
    });
}

void UCesiumRetrievalClient::LoadAsset(int32 AssetId)
{
    FString Url = FString::Printf(TEXT("https://api.cesium.com/v1/assets/%d"), AssetId);
    UE_LOG(LogTemp, Log, TEXT("Loading asset ID: %d from URL: %s"), AssetId, *Url);
    MakeHttpRequest(Url, "GET", "", [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        OnLoadAssetResponse(Request, Response, bWasSuccessful);
    });
}

void UCesiumRetrievalClient::ModifyAsset(int32 AssetId, const FString& NewName, const FString& NewDescription)
{
    FString Url = FString::Printf(TEXT("https://api.cesium.com/v1/assets/%d"), AssetId);

    // Create JSON payload
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField("name", NewName);
    JsonObject->SetStringField("description", NewDescription);

    FString Content;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Log, TEXT("Modifying asset ID: %d with new name: %s and new description: %s"), AssetId, *NewName, *NewDescription);

    MakeHttpRequest(Url, "PATCH", Content, [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        OnModifyAssetResponse(Request, Response, bWasSuccessful);
    });
}

void UCesiumRetrievalClient::DeleteAsset(int32 AssetId)
{
    FString Url = FString::Printf(TEXT("https://api.cesium.com/v1/assets/%d"), AssetId);
    UE_LOG(LogTemp, Log, TEXT("Deleting asset ID: %d from URL: %s"), AssetId, *Url);

    MakeHttpRequest(Url, "DELETE", "", [this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
    {
        OnDeleteAssetResponse(Request, Response, bWasSuccessful);
    });
}

void UCesiumRetrievalClient::OnFetchAssetsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("FetchAssets Response - Success: %d, HTTP Status: %d"), bWasSuccessful, Response->GetResponseCode());
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>>* Items;
            if (JsonObject->TryGetArrayField("items", Items))
            {
                for (auto& Item : *Items)
                {
                    FString AssetName = Item->AsObject()->GetStringField("name");
                    UE_LOG(LogTemp, Log, TEXT("Asset Name: %s"), *AssetName);
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to fetch assets: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

void UCesiumRetrievalClient::OnFetchAssetDetailsResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("FetchAssetDetails Response - Success: %d, HTTP Status: %d"), bWasSuccessful, Response->GetResponseCode());
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString AssetName = JsonObject->GetStringField("name");
            UE_LOG(LogTemp, Log, TEXT("Asset Name: %s"), *AssetName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to fetch asset details: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

void UCesiumRetrievalClient::OnLoadAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("LoadAsset Response - Success: %d, HTTP Status: %d"), bWasSuccessful, Response->GetResponseCode());
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            FString AssetUrl = JsonObject->GetStringField("url");
            UE_LOG(LogTemp, Log, TEXT("Asset URL: %s"), *AssetUrl);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load asset: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

void UCesiumRetrievalClient::OnModifyAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("ModifyAsset Response - Success: %d, HTTP Status: %d"), bWasSuccessful, Response->GetResponseCode());
    if (bWasSuccessful && Response->GetResponseCode() == 200)
    {
        UE_LOG(LogTemp, Log, TEXT("Asset modified successfully: %s"), *Response->GetContentAsString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to modify asset: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

void UCesiumRetrievalClient::OnDeleteAssetResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Log, TEXT("DeleteAsset Response - Success: %d, HTTP Status: %d"), bWasSuccessful, Response->GetResponseCode());
    if (bWasSuccessful && Response->GetResponseCode() == 204)
    {
        UE_LOG(LogTemp, Log, TEXT("Asset deleted successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to delete asset: %d %s"), Response->GetResponseCode(), *Response->GetContentAsString());
    }
}

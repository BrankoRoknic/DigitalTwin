#include "CesiumClient.h"

UCesiumClient::UCesiumClient()
{
	// This field variable contains the access key from Cesium
	fCesiumToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiIwM2MzYTRlNC04MzMzLTRhMDktODVjZS00Mjc0NWRjNGYyNjAiLCJpZCI6MjEzODI0LCJpYXQiOjE3MjE5ODk4MjV9.aDuw8NxL3XgyrWkZ7oqmhX6ImPXJgUG8ZCnxu--UPDs";
}

void UCesiumClient::UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType)
{

	// Get the HTTP module
	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// Create the HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	Request->SetURL("https://api.cesium.com/v1/assets");
	Request->SetVerb("POST");

	// Set headers
	FString token = "Bearer " + this->fCesiumToken;
	Request->SetHeader("Authorization", token);
	Request->SetHeader("Content-Type", "application/json");

	// Set request payload
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", aName);
	JsonObject->SetStringField("description", "");
	JsonObject->SetStringField("type", aConversionType);   // This is specifying what you want cesium to do with the uploaded data - IE create 3D tiles

	TSharedPtr<FJsonObject> OptionsObject = MakeShareable(new FJsonObject);
	OptionsObject->SetStringField("sourceType", aProvidedDataType);  // This is specifying what data you are physically providing to Cesium (.las/.tif etc)
	JsonObject->SetObjectField("options", OptionsObject);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);

	// Bind the response callback
	Request->OnProcessRequestComplete().BindUObject(this, &UCesiumClient::ProvideS3BucketData);

	// Execute the request
	Request->ProcessRequest();

}

void UCesiumClient::ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful)
{
	if (!wasSuccessful || !response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Request Failed"));
		return;
	}

	FString data = response->GetContentAsString();

	UE_LOG(LogTemp, Display, TEXT("HTTP response from Cesium: %s"), *data);
}

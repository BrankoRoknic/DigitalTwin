#pragma once

#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Json.h"
#include "CoreMinimal.h"
#include "CesiumClient.generated.h"

/*
* #include "CoreMinimal.h"
#include "HttpModule.h"

#include "Json.h"
#include "JsonUtilities.h"
#include <string>
#include "UCesiumDataUpload.generated.h"
*/

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumClient : public UObject
{
	GENERATED_BODY()
private:
	FString fCesiumToken;
public:
	UCesiumClient();
	UFUNCTION(BlueprintCallable, Category = "Upload")
	virtual void UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType);
	void ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
};

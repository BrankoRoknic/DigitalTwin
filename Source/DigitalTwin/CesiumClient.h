#pragma once

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
public:
	UCesiumClient();
	UFUNCTION(BlueprintCallable, Category = "Upload")
	virtual void UploadFile(FString aFile, FString aName, FString aConversionType, FString aProvidedDataType);
	void ProvideS3BucketData(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
	void NotifyCesiumUploadComplete(FHttpRequestPtr request, FHttpResponsePtr response, bool wasSuccessful);
};

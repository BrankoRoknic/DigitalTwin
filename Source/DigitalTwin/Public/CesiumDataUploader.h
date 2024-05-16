// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "CesiumDataUploader.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumDataUploader : public UObject
{
	GENERATED_BODY()
	UCesiumDataUploader();

public:
	UFUNCTION(BlueprintCallable, Category="Cesium Ion")
	void UploadToCesiumIon(const FString& FilePath, const FString& AccessToken);

private:
	void OnUploadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};

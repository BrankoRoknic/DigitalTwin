// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "cesiumUploader.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CesiumUploaderBPWrapper.generated.h"

UCLASS(Blueprintable)
class DIGITALTWIN_API UCesiumUploaderBPWrapper : public UObject {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cesium")
	void UploadLasFileToCesium(const FString& FilePath, const FString& AccessToken);

private:
	cesiumUploader Uploader;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "CesiumUploaderBPWrapper.h"


void UCesiumUploaderBPWrapper::UploadLasFileToCesium(const FString& FilePath, const FString& AccessToken) {
	Uploader.UploadLasFileToCesium(FilePath, AccessToken);
}
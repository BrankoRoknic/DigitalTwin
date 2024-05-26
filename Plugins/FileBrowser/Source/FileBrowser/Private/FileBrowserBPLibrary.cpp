// Copyright Epic Games, Inc. All Rights Reserved.

#include "FileBrowserBPLibrary.h"
#include "FileBrowser.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"

UFileBrowserBPLibrary::UFileBrowserBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UFileBrowserBPLibrary::FileBrowserSampleFunction(float Param)
{
	return -1;
}


void UFileBrowserBPLibrary::OpenFilePicker()
{
	// Get the platform-specific file dialog
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform != nullptr && GetWorld() != nullptr)
	{
		TArray<FString> OutFiles;
		bool bFileSelected = DesktopPlatform->OpenFileDialog(
			nullptr,
			TEXT("Select a file"),
			FPaths::ProjectDir(),
			TEXT(""),
			TEXT("All Files (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFiles
		);

		if (bFileSelected && OutFiles.Num() > 0)
		{
			// Call the upload function if a file is selected
			//UploadFile(OutFiles[0]);
			SelectedFilePath = OutFiles[0];
		}
	}
}

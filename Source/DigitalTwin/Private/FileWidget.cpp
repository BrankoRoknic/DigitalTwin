#include "FileWidget.h"
#include "DesktopPlatformModule.h"
#include "CesiumDataUploader.h"
#include "HAL/PlatformFileManager.h"
#include <Misc/FileHelper.h>

void UFileWidget::OpenFilePicker()
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

void UFileWidget::UploadFile(const FString& FilePath) const
{
	if (CesiumDataUploader != nullptr)
	{
		CesiumDataUploader->UploadToCesiumIon(FilePath, TEXT("Your Access Token Here"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CesiumIonUploader reference not set in the blueprint."));
	}
}

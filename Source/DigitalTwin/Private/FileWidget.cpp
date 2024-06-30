#include "FileWidget.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformFileManager.h"
#include <Misc/FileHelper.h>

FString UFileWidget::OpenFilePicker()
{
	// Assuming use of Unreal's desktop platform module to open a file dialog
	FString FilePath;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TArray<FString> OutFiles;
		bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr,
			TEXT("Select a file"),
			FPaths::ProjectDir(),
			TEXT(""),
			TEXT("All Files (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFiles
		);

		if (bOpened && OutFiles.Num() > 0)
		{
			FilePath = OutFiles[0];
		}
	}

	return FilePath;
}
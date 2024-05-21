#include "FileWidget.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformFileManager.h"
#include <Misc/FileHelper.h>

void UFileWidget::OpenFilePicker(FString& OutFilePath)
{
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
			OutFilePath = OutFiles[0];
		}
		else
		{
			OutFilePath = TEXT("");
		}
	}
	else
	{
		OutFilePath = TEXT("");
	}
}

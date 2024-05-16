#pragma once

#include "CoreMinimal.h"
#include "CesiumDataUploader.h"
#include "Blueprint/UserWidget.h"
#include "FileWidget.generated.h"

/**
 * 
 */
UCLASS()
class DIGITALTWIN_API UFileWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Function to open the file dialog and get the file path
	UFUNCTION(BlueprintCallable, Category = "File Picker")
	void OpenFilePicker();

	// Property to store the selected file path
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="File Picker")
	FString SelectedFilePath;
	
	// Reference to the Cesium Ion Uploader class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cesium Ion")
	UCesiumDataUploader* CesiumDataUploader;

protected:
	// Helper function to upload the selected file
	void UploadFile(const FString& FilePath) const;
};

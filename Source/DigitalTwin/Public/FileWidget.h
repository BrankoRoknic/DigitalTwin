#pragma once
#include "Misc/Paths.h"
#include "CoreMinimal.h"
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
	// Static function to open the file dialog and get the file path
	UFUNCTION(BlueprintCallable, Category = "File Picker")
	static void OpenFilePicker(FString& OutFilePath);
};
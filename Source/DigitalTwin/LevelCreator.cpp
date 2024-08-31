// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelCreator.h"
#include "Editor.h"
#include "AssetToolsModule.h"
#include "Factories/WorldFactory.h"
#include "IAssetTools.h"
#include "Misc/PackageName.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "AssetToolsModule.h"
#include <AssetRegistry/AssetRegistryModule.h>



UWorld* ULevelCreator::CreateNewLevel(FString ExistingLevelName, FString NewLevelName)
{
#if WITH_EDITOR
    if (ExistingLevelName.IsEmpty() || NewLevelName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Level names cannot be empty."));
        return nullptr;
    }

    FString ExistingLevelPath = FString::Printf(TEXT("/Game/Dynamic/%s"), *ExistingLevelName);
    FString NewLevelPath = FString::Printf(TEXT("/Game/Dynamic/%s"), *NewLevelName);

    if (!FPackageName::DoesPackageExist(ExistingLevelPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("The level %s does not exist."), *ExistingLevelPath);
        return nullptr;
    }

    if (FPackageName::DoesPackageExist(NewLevelPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("A level with the name %s already exists."), *NewLevelPath);
        return nullptr;
    }

    FString ExistingFilePath = FPackageName::LongPackageNameToFilename(ExistingLevelPath, FPackageName::GetMapPackageExtension());
    FString NewFilePath = FPackageName::LongPackageNameToFilename(NewLevelPath, FPackageName::GetMapPackageExtension());

    if (IFileManager::Get().Copy(*NewFilePath, *ExistingFilePath) != COPY_OK)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to duplicate the level file from %s to %s"), *ExistingFilePath, *NewFilePath);
        return nullptr;
    }

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FString> PathsToScan;
    PathsToScan.Add(FPaths::GetPath(NewFilePath));
    AssetRegistryModule.Get().ScanPathsSynchronous(PathsToScan, true);
    AssetRegistryModule.Get().SearchAllAssets(true);

    UPackage* NewPackage = LoadPackage(nullptr, *NewLevelPath, LOAD_None);
    if (NewPackage)
    {
        UWorld* DuplicatedWorld = UWorld::FindWorldInPackage(NewPackage);
        if (DuplicatedWorld)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully loaded the duplicated level: %s"), *NewLevelName);

            // Saving the duplicated level
            bool bSaved = UPackage::SavePackage(
                NewPackage,
                DuplicatedWorld,
                EObjectFlags::RF_Public | EObjectFlags::RF_Standalone,
                *NewFilePath,
                GError,
                nullptr,
                true,
                true,
                SAVE_NoError
            );

            if (bSaved)
            {
                UE_LOG(LogTemp, Log, TEXT("Successfully saved the duplicated level: %s"), *NewFilePath);
                return DuplicatedWorld;
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to save the duplicated level: %s"), *NewFilePath);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to find UWorld in the duplicated package: %s"), *NewLevelName);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load package for the duplicated level: %s"), *NewLevelName);
    }
#else
    UE_LOG(LogTemp, Error, TEXT("CreateNewLevel can only be used in the editor environment."));
#endif

    return nullptr;
}
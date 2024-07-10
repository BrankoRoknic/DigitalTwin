using System;
using System.Collections.Generic;
using System.IO;
using UnrealBuildTool;

public class AWSSDK : ModuleRules
{
    // list every library you plan to use here
    private List<string> LibraryNames = new List<string>()
    {
        "aws-c-auth",
        "aws-c-cal",
        "aws-c-common",
        "aws-c-compression",
        "aws-c-event-stream",
        "aws-checksums",
        "aws-c-http",
        "aws-c-io",
        "aws-c-mqtt",
        "aws-cpp-sdk-access-management",
        "aws-cpp-sdk-cognito-identity",
        "aws-cpp-sdk-core",
        "aws-cpp-sdk-iam",
        "aws-cpp-sdk-kinesis",
        "aws-crt-cpp",
        "aws-c-s3",
    };

    public AWSSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        // ModuleType.External tells the engine not to look for (or compile) any source code.
        Type = ModuleType.External;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        // add the header files for reference
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Include"));

        // AWS SDK relies on certain identifiers being undefined, so this produces warnings
        // Unreal engine treats certain warnings as errors and fails the build
        // this line will disable those warnings:
        bEnableUndefinedIdentifierWarnings = false;

        // Dynamically linking to the SDK requires us to define the
        // USE_IMPORT_EXPORT symbol for all build targets using the
        // SDK. Source: https://github.com/aws/aws-sdk-cpp/blob/main/Docs/SDK_usage_guide.md#build-defines
        PublicDefinitions.Add("USE_IMPORT_EXPORT");
        PublicDefinitions.Add("AWS_CRT_CPP_USE_IMPORT_EXPORT");
        //PublicDefinitions.Add("AWS_USE_IO_COMPLETION_PORTS");


        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("USE_WINDOWS_DLL_SEMANTICS");
        }

        // do this for each lib and dll
        foreach (string libName in LibraryNames)
        {
            string LibraryPath = Path.Combine(ModuleDirectory, "Binaries", Target.Platform.ToString());
            // add a new section for each platform you plan to compile for (only Win64 is included for this example)
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibraryPath, libName + ".lib"));
                // Stage the library along with the target, so it can be loaded at runtime.
                RuntimeDependencies.Add("$(BinaryOutputDir)/" + libName + ".dll", Path.Combine(LibraryPath, libName + ".dll"));
            }
            else if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                // add mac libraries
            }
            else if (Target.Platform == UnrealTargetPlatform.Linux)
            {
                // add linux libraries
            }
        }
        PrivateDependencyModuleNames.AddRange(
        new string[] {
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "SlateCore",   
        });
    }
}
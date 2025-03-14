// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Gear : ModuleRules
{
	public Gear(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineBase", "Sockets", "UMG", "RenderCore", "Niagara", "HTTP" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.AddRange(new string[] { "Gear" });

        if(Target.Platform == UnrealTargetPlatform.Android)
        {
            var manifestFile = Path.Combine(ModuleDirectory, "AndroidPermissions.xml");
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", manifestFile);
        }

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}

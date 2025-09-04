// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class YUChat : ModuleRules
{
	public YUChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			
			// UI
			"UMG", "Slate", "SlateCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] { "YUChat" });
	}
}

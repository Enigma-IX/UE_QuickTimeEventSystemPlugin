// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class QuickTimeEventsSystem : ModuleRules
{
	public QuickTimeEventsSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
        
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
        
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",        // Besoin pour UObject, UStruct, etc.
				"Engine",             // Besoin pour AActor, UWorld, etc.
				"EnhancedInput",      // Pour UInputAction
				"GameplayTasks",      // IMPORTANT: Pour UGameplayTask !
				"DeveloperSettings",  // Pour UDeveloperSettings
			}
		);
        
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
				// InputCore peut être utile si tu veux des helpers pour FKey
				"InputCore",
			}
		);
        
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Multiplayer_II : ModuleRules
{
	public Multiplayer_II(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Multiplayer_II",
			"Multiplayer_II/Variant_Horror",
			"Multiplayer_II/Variant_Horror/UI",
			"Multiplayer_II/Variant_Shooter",
			"Multiplayer_II/Variant_Shooter/AI",
			"Multiplayer_II/Variant_Shooter/UI",
			"Multiplayer_II/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

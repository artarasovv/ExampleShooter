// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShooterItems : ModuleRules
{
	public ShooterItems(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities", 
			"GameplayTags",
			"GameplayTasks",
			"AbilitySystemUtils"
		});
	}
}
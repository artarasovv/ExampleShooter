// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ExampleShooterCore : ModuleRules
{
	public ExampleShooterCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities", 
			"GameplayTags",
			"GameplayTasks",
			"AbilitySystemUtils",
			"ShooterItems",
			"ShooterCombat"
		});
	}
}

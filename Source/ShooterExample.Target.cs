// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShooterExampleTarget : TargetRules
{
	public ShooterExampleTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[]
		{
			"ExampleShooterCore",
			"AbilitySystemUtils",
			"ShooterCombat",
			"ShooterItems"
		});
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ShooterExampleEditorTarget : TargetRules
{
	public ShooterExampleEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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

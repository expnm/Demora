// exp.dev free

using UnrealBuildTool;
using System.Collections.Generic;

public class DemoraTarget : TargetRules
{
	public DemoraTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Demora" } );
	}
}

// exp.dev free

using UnrealBuildTool;
using System.Collections.Generic;

public class DemoraEditorTarget : TargetRules
{
	public DemoraEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "Demora" } );
	}
}

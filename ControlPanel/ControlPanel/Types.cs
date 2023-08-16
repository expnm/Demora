using System;
using System.Collections.Generic;
using System.Text;

namespace ControlPanel
{
    internal class Types
    {
        public enum EControlPanelEvent
        {
            SwitchFace,
            SwitchHair,
            HideHat,
            HideSkirt,
            HideHands,
            Child,
            Animation1,
            Animation2,
            Animation3,
        };

        public static readonly List<(EControlPanelEvent Event, string Caption)> EventButtons =
            new List<(EControlPanelEvent Event, string Caption)>()
            {
                (EControlPanelEvent.SwitchFace, StringLiterals.SwitchFace),
                (EControlPanelEvent.SwitchHair, StringLiterals.SwitchHair),
                (EControlPanelEvent.HideHat, StringLiterals.HideHat),
                (EControlPanelEvent.HideSkirt, StringLiterals.HideSkirt),
                (EControlPanelEvent.HideHands, StringLiterals.HideHands),
                (EControlPanelEvent.Child, StringLiterals.Child),
                (EControlPanelEvent.Animation1, StringLiterals.Animation1),
                (EControlPanelEvent.Animation2, StringLiterals.Animation2),
                (EControlPanelEvent.Animation3, StringLiterals.Animation3),
            };
    }
}

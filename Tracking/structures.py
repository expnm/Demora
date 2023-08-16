class Point:
    x: float
    y: float

    def __init__(self, x: float = 0, y: float = 0):
        self.x = x
        self.y = y


class HandLandmark:
    # -1:1
    HandRoll = 0
    HandPitch = 1
    HandYaw = 2

    # 0:1
    HandX = 3
    HandY = 4
    HandZ = 5

    # 0:1
    IndexFinger = 6
    MiddleFinger = 7
    RingFinger = 8
    PinkyFinger = 9
    ThumbFinger = 10

    MAX = 11


class FaceExtendedData:
    HeadYaw: float = 0
    HeadPitch: float = 0
    HeadRoll: float = 0
    LeftEyeYaw: float = 0
    LeftEyePitch: float = 0
    RightEyeYaw: float = 0
    RightEyePitch: float = 0

    def round(self, accuracy: int = 2):
        self.HeadYaw = round(self.HeadYaw, accuracy)
        self.HeadPitch = round(self.HeadPitch, accuracy)
        self.HeadRoll = round(self.HeadRoll, accuracy)
        self.LeftEyeYaw = round(self.LeftEyeYaw, accuracy)
        self.LeftEyePitch = round(self.LeftEyePitch, accuracy)
        self.RightEyeYaw = round(self.RightEyeYaw, accuracy)
        self.RightEyePitch = round(self.RightEyePitch, accuracy)


class FaceTrackingInfo:
    class MediapipeLandmark:
        Upper = 9
        Lower = 0
        Left = 263
        Right = 33

    class ArKitBlendShape:
        EyeLookDownLeft = 1
        EyeLookInLeft = 2
        EyeLookOutLeft = 3
        EyeLookUpLeft = 4
        EyeLookDownRight = 8
        EyeLookInRight = 9
        EyeLookOutRight = 10
        EyeLookUpRight = 11
        HeadYaw = 52
        HeadPitch = 53
        HeadRoll = 54
        LeftEyeYaw = 55
        LeftEyePitch = 56
        LeftEyeRoll = 57
        RightEyeYaw = 58
        RightEyePitch = 59
        RightEyeRoll = 60

    class BlendShapeConnection:
        Name: str
        Mediapipe: int
        ArKit: int

        def __init__(self, name: str, mediapipe: int, arkit: int):
            self.Name = name
            self.Mediapipe = mediapipe
            self.ArKit = arkit

    face_blendshape_connections = [
        BlendShapeConnection(name='browDownLeft', mediapipe=1, arkit=41),
        BlendShapeConnection(name='browDownRight', mediapipe=2, arkit=42),
        BlendShapeConnection(name='browInnerUp', mediapipe=3, arkit=43),
        BlendShapeConnection(name='browOuterUpLeft', mediapipe=4, arkit=44),
        BlendShapeConnection(name='browOuterUpRight', mediapipe=5, arkit=45),
        BlendShapeConnection(name='cheekPuff', mediapipe=6, arkit=46),
        BlendShapeConnection(name='cheekSquintLeft', mediapipe=7, arkit=47),
        BlendShapeConnection(name='cheekSquintRight', mediapipe=8, arkit=48),
        BlendShapeConnection(name='eyeBlinkLeft', mediapipe=9, arkit=0),
        BlendShapeConnection(name='eyeBlinkRight', mediapipe=10, arkit=7),
        BlendShapeConnection(name='eyeLookDownLeft', mediapipe=11, arkit=ArKitBlendShape.EyeLookDownLeft),
        BlendShapeConnection(name='eyeLookDownRight', mediapipe=12, arkit=ArKitBlendShape.EyeLookDownRight),
        BlendShapeConnection(name='eyeLookInLeft', mediapipe=13, arkit=ArKitBlendShape.EyeLookInLeft),
        BlendShapeConnection(name='eyeLookInRight', mediapipe=14, arkit=ArKitBlendShape.EyeLookInRight),
        BlendShapeConnection(name='eyeLookOutLeft', mediapipe=15, arkit=ArKitBlendShape.EyeLookOutLeft),
        BlendShapeConnection(name='eyeLookOutRight', mediapipe=16, arkit=ArKitBlendShape.EyeLookOutRight),
        BlendShapeConnection(name='eyeLookUpLeft', mediapipe=17, arkit=ArKitBlendShape.EyeLookUpLeft),
        BlendShapeConnection(name='eyeLookUpRight', mediapipe=18, arkit=ArKitBlendShape.EyeLookUpRight),
        BlendShapeConnection(name='eyeSquintLeft', mediapipe=19, arkit=5),
        BlendShapeConnection(name='eyeSquintRight', mediapipe=20, arkit=12),
        BlendShapeConnection(name='eyeWideLeft', mediapipe=21, arkit=6),
        BlendShapeConnection(name='eyeWideRight', mediapipe=22, arkit=13),
        BlendShapeConnection(name='jawForward', mediapipe=23, arkit=14),
        BlendShapeConnection(name='jawLeft', mediapipe=24, arkit=15),
        BlendShapeConnection(name='jawOpen', mediapipe=25, arkit=17),
        BlendShapeConnection(name='jawRight', mediapipe=26, arkit=16),
        BlendShapeConnection(name='mouthClose', mediapipe=27, arkit=18),
        BlendShapeConnection(name='mouthDimpleLeft', mediapipe=28, arkit=27),
        BlendShapeConnection(name='mouthDimpleRight', mediapipe=29, arkit=28),
        BlendShapeConnection(name='mouthFrownLeft', mediapipe=30, arkit=25),
        BlendShapeConnection(name='mouthFrownRight', mediapipe=31, arkit=26),
        BlendShapeConnection(name='mouthFunnel', mediapipe=32, arkit=19),
        BlendShapeConnection(name='mouthLeft', mediapipe=33, arkit=21),
        BlendShapeConnection(name='mouthLowerDownLeft', mediapipe=34, arkit=37),
        BlendShapeConnection(name='mouthLowerDownRight', mediapipe=35, arkit=38),
        BlendShapeConnection(name='mouthPressLeft', mediapipe=36, arkit=35),
        BlendShapeConnection(name='mouthPressRight', mediapipe=37, arkit=36),
        BlendShapeConnection(name='mouthPucker', mediapipe=38, arkit=20),
        BlendShapeConnection(name='mouthRight', mediapipe=39, arkit=22),
        BlendShapeConnection(name='mouthRollLower', mediapipe=40, arkit=31),
        BlendShapeConnection(name='mouthRollUpper', mediapipe=41, arkit=32),
        BlendShapeConnection(name='mouthShrugLower', mediapipe=42, arkit=33),
        BlendShapeConnection(name='mouthShrugUpper', mediapipe=43, arkit=34),
        BlendShapeConnection(name='mouthSmileLeft', mediapipe=44, arkit=23),
        BlendShapeConnection(name='mouthSmileRight', mediapipe=45, arkit=24),
        BlendShapeConnection(name='mouthStretchLeft', mediapipe=46, arkit=29),
        BlendShapeConnection(name='mouthStretchRight', mediapipe=47, arkit=30),
        BlendShapeConnection(name='mouthUpperUpLeft', mediapipe=48, arkit=39),
        BlendShapeConnection(name='mouthUpperUpRight', mediapipe=49, arkit=40),
        BlendShapeConnection(name='noseSneerLeft', mediapipe=50, arkit=49),
        BlendShapeConnection(name='noseSneerRight', mediapipe=51, arkit=50),
        BlendShapeConnection(name='tongueOut', mediapipe=52, arkit=51),
    ]

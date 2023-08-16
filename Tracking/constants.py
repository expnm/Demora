class Common:
    SCREEN_WIDTH = 1920
    SCREEN_HEIGHT = 1080


class Connection:
    IP = "192.168.0.104"
    FACE_PORT = 4005
    HAND_PORT = 4006


class FaceTracking:
    HEAD_ROLL_MIN = -60
    HEAD_ROLL_MAX = 60

    HEAD_PITCH_MIN = 0.0
    HEAD_PITCH_MAX = 0.5


class HandTracking:
    HAND_ROLL_MIN = -40
    HAND_ROLL_MAX = 40
    HAND_YAW_MIN = -220
    HAND_YAW_MAX = -140
    HAND_PITCH_MIN = 0.65
    HAND_PITCH_MAX = 1.25

    HAND_Y_MIN = 0.8
    HAND_Y_MAX = 2
    HAND_Z_MIN = -0.1
    HAND_Z_MAX = 0.8

    INDEX_MIN = 0.2
    INDEX_MAX = 0.8
    MIDDLE_MIN = 0.2
    MIDDLE_MAX = 0.8
    RING_MIN = 0.2
    RING_MAX = 0.8
    PINKY_MIN = 0.2
    PINKY_MAX = 0.65
    THUMB_MIN = 0.15
    THUMB_MAX = 0.65
    THUMB_SIDE_MIN = 0.3
    THUMB_SIDE_MAX = 0.5

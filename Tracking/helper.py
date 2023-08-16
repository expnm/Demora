import mediapipe
from mediapipe.tasks.python.components.containers.landmark import NormalizedLandmark
import math
import socket
import numpy as np

import constants as const
from structures import Point
from structures import FaceTrackingInfo
from structures import FaceExtendedData
from structures import HandLandmark as OutHandLandmark

HandLandmark = mediapipe.solutions.hands.HandLandmark
_init_palm_len: float = 0


def calculate_angle(p1: Point, p2: Point, p3: Point) -> float:
    angle_rad = np.arctan2(p3.y - p2.y, p3.x - p2.x) - np.arctan2(p1.y - p2.y, p1.x - p2.x)
    return math.degrees(angle_rad)


def calculate_length(p1: Point, p2: Point) -> float:
    return math.sqrt((p1.x - p2.x) ** 2 + (p1.y - p2.y) ** 2)


def clamp(value: float, min_value: float = 0, max_value: float = 1) -> float:
    return max(min(value, max_value), min_value)


def convert_list_to_str(input_list: list[float], accuracy: int = 2) -> str:
    result: str = ''
    for value in input_list:
        result += f'{round(value, accuracy)},'
    if len(result) > 0:
        result = result[:-1]
    return result


def find_face_mediapipe_index(arkit_index: int) -> int:
    for face_blendshape_connection in FaceTrackingInfo.face_blendshape_connections:
        if face_blendshape_connection.ArKit == arkit_index:
            return face_blendshape_connection.Mediapipe
    return -1


def convert_face_mediapipe_to_arkit(mediapipe_face_blendshapes: list[float]) -> list[float]:
    arkit_face_blendshapes: list[float] = []
    arkit_face_blendshapes_count = 61
    for arkit_index in range(arkit_face_blendshapes_count):
        mediapipe_index: int = find_face_mediapipe_index(arkit_index)
        value: float = 0
        if 0 <= mediapipe_index < len(mediapipe_face_blendshapes):
            value = mediapipe_face_blendshapes[mediapipe_index]
        arkit_face_blendshapes.append(value)
    return arkit_face_blendshapes


def calculate_extended_face_blendshapes(upper_point: Point, lower_point: Point,
                                        left_point: Point, right_point: Point,
                                        arkit_face_blendshapes: list[float], accuracy: int = 2) \
        -> FaceExtendedData:
    result = FaceExtendedData()

    head_roll_angle = calculate_angle(upper_point, lower_point, Point(lower_point.x, -100))
    result.HeadRoll = np.interp(
        head_roll_angle,
        [const.FaceTracking.HEAD_ROLL_MIN, const.FaceTracking.HEAD_ROLL_MAX],
        [1, -1])

    middle_point = Point(
        lower_point.x + (upper_point.x - lower_point.x) * 0.5,
        left_point.y + (right_point.y - left_point.y) * 0.5)

    upper_lower_len = calculate_length(upper_point, lower_point)
    upper_middle_len = calculate_length(upper_point, middle_point)
    result.HeadPitch = np.interp(
        upper_middle_len / upper_lower_len,
        [const.FaceTracking.HEAD_PITCH_MIN, const.FaceTracking.HEAD_PITCH_MAX],
        [-1, 1])

    left_right_len = calculate_length(left_point, right_point)
    left_middle_len = calculate_length(left_point, middle_point)
    right_middle_len = calculate_length(right_point, middle_point)
    result.HeadYaw = clamp((left_middle_len - right_middle_len) / left_right_len, -1, 1)

    eye_pitch_multiply = 0.5
    eye_yaw_multiply = 0.5

    result.LeftEyePitch = clamp(
        (arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookDownLeft] -
         arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookUpLeft]) * eye_pitch_multiply,
        -1, 1)

    result.LeftEyeYaw = clamp(
        (arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookOutLeft] -
         arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookInLeft]) * eye_yaw_multiply,
        -1, 1)

    result.RightEyePitch = clamp(
        (arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookDownRight] -
         arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookUpRight]) * eye_pitch_multiply,
        -1, 1)

    result.RightEyeYaw = clamp(
        (arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookInRight] -
         arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.EyeLookOutRight]) * eye_yaw_multiply,
        -1, 1)

    result.round(accuracy)
    return result


def get_hand_landmark_by_side(hand_landmarks: list[NormalizedLandmark], hand_landmark: int, is_right: bool = False) \
        -> Point:
    x = hand_landmarks[hand_landmark].x * const.Common.SCREEN_WIDTH
    if is_right:
        x = 1 - x
    y = (1 - hand_landmarks[hand_landmark].y) * const.Common.SCREEN_HEIGHT
    return Point(x, y)


def calculate_hand_landmarks(hand_landmarks: list[NormalizedLandmark], is_right: bool = False, accuracy: int = 2) \
        -> list[float]:
    # init var
    result: list[float] = [0] * OutHandLandmark.MAX

    wrist = get_hand_landmark_by_side(hand_landmarks, HandLandmark.WRIST, is_right)
    index_mcp = get_hand_landmark_by_side(hand_landmarks, HandLandmark.INDEX_FINGER_MCP, is_right)
    index_tip = get_hand_landmark_by_side(hand_landmarks, HandLandmark.INDEX_FINGER_TIP, is_right)
    middle_mcp = get_hand_landmark_by_side(hand_landmarks, HandLandmark.MIDDLE_FINGER_MCP, is_right)
    middle_tip = get_hand_landmark_by_side(hand_landmarks, HandLandmark.MIDDLE_FINGER_TIP, is_right)
    ring_mcp = get_hand_landmark_by_side(hand_landmarks, HandLandmark.RING_FINGER_MCP, is_right)
    ring_tip = get_hand_landmark_by_side(hand_landmarks, HandLandmark.RING_FINGER_TIP, is_right)
    pinky_mcp = get_hand_landmark_by_side(hand_landmarks, HandLandmark.PINKY_MCP, is_right)
    pinky_tip = get_hand_landmark_by_side(hand_landmarks, HandLandmark.PINKY_TIP, is_right)
    thumb_mcp = get_hand_landmark_by_side(hand_landmarks, HandLandmark.THUMB_MCP, is_right)
    thumb_tip = get_hand_landmark_by_side(hand_landmarks, HandLandmark.THUMB_TIP, is_right)

    palm_width = calculate_length(index_mcp, pinky_mcp)
    palm_len = calculate_length(wrist, middle_mcp)

    index_len = calculate_length(index_mcp, index_tip)
    middle_len = calculate_length(middle_mcp, middle_tip)
    ring_len = calculate_length(ring_mcp, ring_tip)
    pinky_len = calculate_length(pinky_mcp, pinky_tip)
    thumb_len = calculate_length(thumb_mcp, thumb_tip)
    index_thumb_len = calculate_length(index_mcp, thumb_tip)

    global _init_palm_len
    if _init_palm_len == 0:
        _init_palm_len = palm_len

    # hand rotate
    hand_roll_angle = calculate_angle(index_mcp, wrist, pinky_mcp)
    result[OutHandLandmark.HandRoll] = np.interp(
        hand_roll_angle,
        [const.HandTracking.HAND_ROLL_MIN, const.HandTracking.HAND_ROLL_MAX],
        [-1, 1])

    hand_yaw_angle = calculate_angle(middle_mcp, wrist, Point(wrist.x, -100))
    result[OutHandLandmark.HandYaw] = np.interp(
        hand_yaw_angle,
        [const.HandTracking.HAND_YAW_MIN, const.HandTracking.HAND_YAW_MAX],
        [1, -1])

    is_palm_visible = math.fabs(result[OutHandLandmark.HandRoll]) > 0.5
    if is_palm_visible:
        hand_pitch_sign = -1 if hand_landmarks[HandLandmark.MIDDLE_FINGER_MCP].z > 0 else 1
        result[OutHandLandmark.HandPitch] = hand_pitch_sign * np.interp(
            math.fabs(palm_width / palm_len),
            [const.HandTracking.HAND_PITCH_MIN, const.HandTracking.HAND_PITCH_MAX],
            [0, 1])
    else:
        result[OutHandLandmark.HandPitch] = result[OutHandLandmark.HandYaw] * -1
        result[OutHandLandmark.HandYaw] = 0

    # hand move
    wrist_unscale_x = (1 - hand_landmarks[HandLandmark.WRIST].x) if is_right else hand_landmarks[HandLandmark.WRIST].x
    result[OutHandLandmark.HandX] = clamp(wrist_unscale_x)

    result[OutHandLandmark.HandZ] = np.interp(
        1 - hand_landmarks[HandLandmark.WRIST].y,
        [const.HandTracking.HAND_Z_MIN, const.HandTracking.HAND_Z_MAX],
        [0, 1])

    palm_height_ratio = np.interp(
        palm_len / _init_palm_len,
        [const.HandTracking.HAND_Y_MIN, const.HandTracking.HAND_Y_MAX],
        [0, 1])
    hand_y_error = (result[OutHandLandmark.HandPitch] * 0.5) if is_palm_visible else 0
    result[OutHandLandmark.HandY] = clamp(palm_height_ratio + hand_y_error)

    # fingers
    result[OutHandLandmark.IndexFinger] = np.interp(
        index_len / palm_len,
        [const.HandTracking.INDEX_MIN, const.HandTracking.INDEX_MAX],
        [1, 0])

    result[OutHandLandmark.MiddleFinger] = np.interp(
        middle_len / palm_len,
        [const.HandTracking.MIDDLE_MIN, const.HandTracking.MIDDLE_MAX],
        [1, 0])

    result[OutHandLandmark.RingFinger] = np.interp(
        ring_len / palm_len,
        [const.HandTracking.RING_MIN, const.HandTracking.RING_MAX],
        [1, 0])

    result[OutHandLandmark.PinkyFinger] = np.interp(
        pinky_len / palm_len,
        [const.HandTracking.PINKY_MIN, const.HandTracking.PINKY_MAX],
        [1, 0])

    if is_palm_visible:
        result[OutHandLandmark.ThumbFinger] = np.interp(
            index_thumb_len / palm_len,
            [const.HandTracking.THUMB_MIN, const.HandTracking.THUMB_MAX],
            [1, 0])
    else:
        result[OutHandLandmark.ThumbFinger] = np.interp(
            thumb_len / palm_len,
            [const.HandTracking.THUMB_SIDE_MIN, const.HandTracking.THUMB_SIDE_MAX],
            [1, 0])

    for i in range(len(result)):
        result[i] = round(result[i], accuracy)

    return result


def send_message(message: str, soc: socket.socket, soc_adr: tuple[str, int]):
    soc.sendto(message.encode(), soc_adr)

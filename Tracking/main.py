import time

import mediapipe as mp
from mediapipe.tasks.python.components.containers.landmark import NormalizedLandmark
import cv2
import socket

import numpy

import helper
from constants import Common
from constants import Connection
from structures import Point
from structures import FaceTrackingInfo
from structures import HandLandmark as OutHandLandmark

SHOW_CAM: bool = True

BaseOptions = mp.tasks.BaseOptions
VisionRunningMode = mp.tasks.vision.RunningMode

# global
_head_points = []
_head_points_offset = []
_eye_r = []
_eye_l = []
_mouth_upper = []
_mouth_lower = []
_mouth_inner = []
_brow_l = []
_brow_r = []
_eyes = []

# face
face_soc: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
face_soc_adr: tuple[str, int] = (Connection.IP, Connection.FACE_PORT)

FaceLandmarker = mp.tasks.vision.FaceLandmarker
FaceLandmarkerOptions = mp.tasks.vision.FaceLandmarkerOptions
FaceLandmarkerResult = mp.tasks.vision.FaceLandmarkerResult


def face_callback(result: FaceLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
    if result.face_blendshapes and result.face_landmarks:
        # get and convert mediapipe result
        face_blendshapes = [face_blendshape.score for face_blendshape in result.face_blendshapes[0]]
        arkit_face_blendshapes = helper.convert_face_mediapipe_to_arkit(face_blendshapes)

        # get extended blendshapes
        upper_point = Point(
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Upper].x * Common.SCREEN_WIDTH,
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Upper].y * Common.SCREEN_HEIGHT)
        lower_point = Point(
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Lower].x * Common.SCREEN_WIDTH,
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Lower].y * Common.SCREEN_HEIGHT)
        left_point = Point(
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Left].x * Common.SCREEN_WIDTH,
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Left].y * Common.SCREEN_HEIGHT)
        right_point = Point(
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Right].x * Common.SCREEN_WIDTH,
            result.face_landmarks[0][FaceTrackingInfo.MediapipeLandmark.Right].y * Common.SCREEN_HEIGHT)

        extended_blendshapes = helper.calculate_extended_face_blendshapes(upper_point, lower_point,
                                                                          left_point, right_point,
                                                                          arkit_face_blendshapes)

        # add extended blendshapes
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.HeadRoll] = extended_blendshapes.HeadRoll
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.HeadPitch] = extended_blendshapes.HeadPitch
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.HeadYaw] = extended_blendshapes.HeadYaw
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.LeftEyePitch] = extended_blendshapes.LeftEyePitch
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.LeftEyeYaw] = extended_blendshapes.LeftEyeYaw
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.RightEyePitch] = extended_blendshapes.RightEyePitch
        arkit_face_blendshapes[FaceTrackingInfo.ArKitBlendShape.RightEyeYaw] = extended_blendshapes.RightEyeYaw

        # convert to str and send
        arkit_face_blendshapes_message = helper.convert_list_to_str(arkit_face_blendshapes)
        # print(arkit_face_blendshapes_message)
        helper.send_message(arkit_face_blendshapes_message, face_soc, face_soc_adr)

        offset = 75
        global _head_points, _head_points_offset, _eye_l, _eye_r, _mouth_upper, _mouth_lower, _brow_l, _brow_r, _eyes, _mouth_inner
        _head_points = [get_face_point(result.face_landmarks[0][10]),
                        get_face_point(result.face_landmarks[0][109]),
                        get_face_point(result.face_landmarks[0][67]),
                        get_face_point(result.face_landmarks[0][103]),
                        get_face_point(result.face_landmarks[0][54]),
                        get_face_point(result.face_landmarks[0][21]),
                        get_face_point(result.face_landmarks[0][162]),
                        get_face_point(result.face_landmarks[0][127]),
                        get_face_point(result.face_landmarks[0][93]),
                        get_face_point(result.face_landmarks[0][132]),
                        get_face_point(result.face_landmarks[0][58]),
                        get_face_point(result.face_landmarks[0][172]),
                        get_face_point(result.face_landmarks[0][136]),
                        get_face_point(result.face_landmarks[0][150]),
                        get_face_point(result.face_landmarks[0][149]),
                        get_face_point(result.face_landmarks[0][176]),
                        get_face_point(result.face_landmarks[0][148]),
                        get_face_point(result.face_landmarks[0][152]),
                        get_face_point(result.face_landmarks[0][377]),
                        get_face_point(result.face_landmarks[0][400]),
                        get_face_point(result.face_landmarks[0][378]),
                        get_face_point(result.face_landmarks[0][379]),
                        get_face_point(result.face_landmarks[0][365]),
                        get_face_point(result.face_landmarks[0][397]),
                        get_face_point(result.face_landmarks[0][288]),
                        get_face_point(result.face_landmarks[0][361]),
                        get_face_point(result.face_landmarks[0][323]),
                        get_face_point(result.face_landmarks[0][356]),
                        get_face_point(result.face_landmarks[0][389]),
                        get_face_point(result.face_landmarks[0][251]),
                        get_face_point(result.face_landmarks[0][284]),
                        get_face_point(result.face_landmarks[0][332]),
                        get_face_point(result.face_landmarks[0][297]),
                        get_face_point(result.face_landmarks[0][338])]

        _head_points_offset = [get_face_point(result.face_landmarks[0][10], 0, -offset),
                               get_face_point(result.face_landmarks[0][109], 0, -offset),
                               get_face_point(result.face_landmarks[0][67], 0, -offset),
                               get_face_point(result.face_landmarks[0][103], -offset, -offset),
                               get_face_point(result.face_landmarks[0][54], -offset, -offset),
                               get_face_point(result.face_landmarks[0][21], -offset),
                               get_face_point(result.face_landmarks[0][162], -offset),
                               get_face_point(result.face_landmarks[0][127], -offset),
                               get_face_point(result.face_landmarks[0][93], -offset),
                               get_face_point(result.face_landmarks[0][132], -offset),
                               get_face_point(result.face_landmarks[0][58], -offset),
                               get_face_point(result.face_landmarks[0][172], -offset),
                               get_face_point(result.face_landmarks[0][136], -offset, offset),
                               get_face_point(result.face_landmarks[0][150], -offset, offset),
                               get_face_point(result.face_landmarks[0][149], -offset, offset),
                               get_face_point(result.face_landmarks[0][176], -offset, offset),
                               get_face_point(result.face_landmarks[0][148], -offset, offset),
                               get_face_point(result.face_landmarks[0][152], 0, offset),
                               get_face_point(result.face_landmarks[0][377], offset, offset),
                               get_face_point(result.face_landmarks[0][400], offset, offset),
                               get_face_point(result.face_landmarks[0][378], offset, offset),
                               get_face_point(result.face_landmarks[0][379], offset, offset),
                               get_face_point(result.face_landmarks[0][365], offset, offset),
                               get_face_point(result.face_landmarks[0][397], offset),
                               get_face_point(result.face_landmarks[0][288], offset),
                               get_face_point(result.face_landmarks[0][361], offset),
                               get_face_point(result.face_landmarks[0][323], offset),
                               get_face_point(result.face_landmarks[0][356], offset),
                               get_face_point(result.face_landmarks[0][389], offset),
                               get_face_point(result.face_landmarks[0][251], offset),
                               get_face_point(result.face_landmarks[0][284], offset, -offset),
                               get_face_point(result.face_landmarks[0][332], offset, -offset),
                               get_face_point(result.face_landmarks[0][297], 0, -offset),
                               get_face_point(result.face_landmarks[0][338], 0, -offset)]

        _eye_l = [get_face_point(result.face_landmarks[0][263]),
                  get_face_point(result.face_landmarks[0][466]),
                  get_face_point(result.face_landmarks[0][388]),
                  get_face_point(result.face_landmarks[0][387]),
                  get_face_point(result.face_landmarks[0][386]),
                  get_face_point(result.face_landmarks[0][385]),
                  get_face_point(result.face_landmarks[0][384]),
                  get_face_point(result.face_landmarks[0][398]),
                  get_face_point(result.face_landmarks[0][362]),
                  get_face_point(result.face_landmarks[0][382]),
                  get_face_point(result.face_landmarks[0][381]),
                  get_face_point(result.face_landmarks[0][380]),
                  get_face_point(result.face_landmarks[0][374]),
                  get_face_point(result.face_landmarks[0][373]),
                  get_face_point(result.face_landmarks[0][390]),
                  get_face_point(result.face_landmarks[0][249])]

        _eye_r = [get_face_point(result.face_landmarks[0][33]),
                  get_face_point(result.face_landmarks[0][246]),
                  get_face_point(result.face_landmarks[0][161]),
                  get_face_point(result.face_landmarks[0][160]),
                  get_face_point(result.face_landmarks[0][159]),
                  get_face_point(result.face_landmarks[0][158]),
                  get_face_point(result.face_landmarks[0][157]),
                  get_face_point(result.face_landmarks[0][173]),
                  get_face_point(result.face_landmarks[0][133]),
                  get_face_point(result.face_landmarks[0][155]),
                  get_face_point(result.face_landmarks[0][154]),
                  get_face_point(result.face_landmarks[0][153]),
                  get_face_point(result.face_landmarks[0][145]),
                  get_face_point(result.face_landmarks[0][144]),
                  get_face_point(result.face_landmarks[0][163]),
                  get_face_point(result.face_landmarks[0][7])]

        _mouth_upper = [get_face_point(result.face_landmarks[0][78]),
                        get_face_point(result.face_landmarks[0][191]),
                        get_face_point(result.face_landmarks[0][80]),
                        get_face_point(result.face_landmarks[0][81]),
                        get_face_point(result.face_landmarks[0][82]),
                        get_face_point(result.face_landmarks[0][13]),
                        get_face_point(result.face_landmarks[0][312]),
                        get_face_point(result.face_landmarks[0][311]),
                        get_face_point(result.face_landmarks[0][310]),
                        get_face_point(result.face_landmarks[0][415]),
                        get_face_point(result.face_landmarks[0][308]),
                        get_face_point(result.face_landmarks[0][291]),
                        get_face_point(result.face_landmarks[0][409]),
                        get_face_point(result.face_landmarks[0][270]),
                        get_face_point(result.face_landmarks[0][269]),
                        get_face_point(result.face_landmarks[0][267]),
                        get_face_point(result.face_landmarks[0][0]),
                        get_face_point(result.face_landmarks[0][37]),
                        get_face_point(result.face_landmarks[0][39]),
                        get_face_point(result.face_landmarks[0][40]),
                        get_face_point(result.face_landmarks[0][185]),
                        get_face_point(result.face_landmarks[0][61])]

        _mouth_lower = [get_face_point(result.face_landmarks[0][78]),
                        get_face_point(result.face_landmarks[0][95]),
                        get_face_point(result.face_landmarks[0][88]),
                        get_face_point(result.face_landmarks[0][178]),
                        get_face_point(result.face_landmarks[0][87]),
                        get_face_point(result.face_landmarks[0][14]),
                        get_face_point(result.face_landmarks[0][317]),
                        get_face_point(result.face_landmarks[0][402]),
                        get_face_point(result.face_landmarks[0][318]),
                        get_face_point(result.face_landmarks[0][324]),
                        get_face_point(result.face_landmarks[0][308]),
                        get_face_point(result.face_landmarks[0][291]),
                        get_face_point(result.face_landmarks[0][375]),
                        get_face_point(result.face_landmarks[0][321]),
                        get_face_point(result.face_landmarks[0][405]),
                        get_face_point(result.face_landmarks[0][314]),
                        get_face_point(result.face_landmarks[0][17]),
                        get_face_point(result.face_landmarks[0][84]),
                        get_face_point(result.face_landmarks[0][181]),
                        get_face_point(result.face_landmarks[0][91]),
                        get_face_point(result.face_landmarks[0][146]),
                        get_face_point(result.face_landmarks[0][61])]

        _mouth_inner = [get_face_point(result.face_landmarks[0][78]),
                        get_face_point(result.face_landmarks[0][191]),
                        get_face_point(result.face_landmarks[0][80]),
                        get_face_point(result.face_landmarks[0][81]),
                        get_face_point(result.face_landmarks[0][82]),
                        get_face_point(result.face_landmarks[0][13]),
                        get_face_point(result.face_landmarks[0][312]),
                        get_face_point(result.face_landmarks[0][311]),
                        get_face_point(result.face_landmarks[0][310]),
                        get_face_point(result.face_landmarks[0][415]),
                        get_face_point(result.face_landmarks[0][308]),
                        get_face_point(result.face_landmarks[0][324]),
                        get_face_point(result.face_landmarks[0][318]),
                        get_face_point(result.face_landmarks[0][402]),
                        get_face_point(result.face_landmarks[0][317]),
                        get_face_point(result.face_landmarks[0][14]),
                        get_face_point(result.face_landmarks[0][87]),
                        get_face_point(result.face_landmarks[0][178]),
                        get_face_point(result.face_landmarks[0][88]),
                        get_face_point(result.face_landmarks[0][95])]

        _brow_l = [get_face_point(result.face_landmarks[0][300]),
                   get_face_point(result.face_landmarks[0][293]),
                   get_face_point(result.face_landmarks[0][334]),
                   get_face_point(result.face_landmarks[0][296]),
                   get_face_point(result.face_landmarks[0][336])]

        _brow_r = [get_face_point(result.face_landmarks[0][70]),
                   get_face_point(result.face_landmarks[0][63]),
                   get_face_point(result.face_landmarks[0][105]),
                   get_face_point(result.face_landmarks[0][66]),
                   get_face_point(result.face_landmarks[0][107])]

        _eyes = [get_face_point(result.face_landmarks[0][468]),
                 get_face_point(result.face_landmarks[0][473])]


def get_face_point(landmark: NormalizedLandmark, x=0, y=0):
    return int(landmark.x * Common.SCREEN_WIDTH) + x, int(landmark.y * Common.SCREEN_HEIGHT) + y


face_options = FaceLandmarkerOptions(
    base_options=BaseOptions(model_asset_path="face_landmarker.task"),
    running_mode=VisionRunningMode.LIVE_STREAM,
    result_callback=face_callback,
    output_face_blendshapes=True)

# hands
hand_soc: socket.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
hand_soc_adr: tuple[str, int] = (Connection.IP, Connection.HAND_PORT)

HandLandmarker = mp.tasks.vision.HandLandmarker
HandLandmarkerOptions = mp.tasks.vision.HandLandmarkerOptions
HandLandmarkerResult = mp.tasks.vision.HandLandmarkerResult


_left_hand_point = []
_right_hand_point = []


def hands_callback(result: HandLandmarkerResult, output_image: mp.Image, timestamp_ms: int):
    if result.hand_landmarks:
        if (len(result.handedness) == 2 and
                result.handedness[0][0].category_name ==
                result.handedness[1][0].category_name):
            return

        left_hand_landmarks: list[float] = [0] * OutHandLandmark.MAX
        right_hand_landmarks: list[float] = [0] * OutHandLandmark.MAX

        global _left_hand_point, _right_hand_point

        for i in range(len(result.handedness)):
            if result.handedness[i][0].category_name.lower() == 'right':
                right_hand_landmarks = helper.calculate_hand_landmarks(result.hand_landmarks[i], is_right=True)
                _right_hand_point = []
                for j in range(21):
                    _right_hand_point.append(get_face_point(result.hand_landmarks[i][j]))
            else:
                left_hand_landmarks = helper.calculate_hand_landmarks(result.hand_landmarks[i], is_right=False)
                _left_hand_point = []
                for j in range(21):
                    _left_hand_point.append(get_face_point(result.hand_landmarks[i][j]))

        # prepare and send results
        if left_hand_landmarks == right_hand_landmarks == []:
            return

        hand_landmarks_message = ''.join(f'{lm},' for lm in left_hand_landmarks)
        hand_landmarks_message += ''.join(f'{lm},' for lm in right_hand_landmarks)
        hand_landmarks_message = hand_landmarks_message[:-1]
        # print(hand_landmarks_message)
        helper.send_message(hand_landmarks_message, hand_soc, hand_soc_adr)


hands_options = HandLandmarkerOptions(
    base_options=BaseOptions(model_asset_path='hand_landmarker.task'),
    running_mode=VisionRunningMode.LIVE_STREAM,
    result_callback=hands_callback,
    num_hands=2)

# tracking
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, Common.SCREEN_WIDTH)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, Common.SCREEN_HEIGHT)
with FaceLandmarker.create_from_options(face_options) as face_landmarker:
    with HandLandmarker.create_from_options(hands_options) as hands_landmarker:
        while cap.isOpened():
            success, image = cap.read()
            if not success:
                continue

            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            image = cv2.flip(image, 1)
            mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data=image)

            timestamp = round(time.time() * 1000)

            face_landmarker.detect_async(mp_image, timestamp)
            hands_landmarker.detect_async(mp_image, timestamp)

            if SHOW_CAM:
                if _head_points and _head_points_offset:
                    cv2.fillPoly(image, numpy.array([_head_points_offset]), (0, 0, 0))
                    cv2.fillPoly(image, numpy.array([_mouth_upper]), (255, 255, 255))
                    cv2.fillPoly(image, numpy.array([_mouth_lower]), (255, 255, 255))
                    cv2.fillPoly(image, numpy.array([_mouth_inner]), (255, 0, 0))
                    cv2.fillPoly(image, numpy.array([_brow_l]), (255, 255, 255))
                    cv2.fillPoly(image, numpy.array([_brow_r]), (255, 255, 255))
                    cv2.circle(image, _eyes[0], 10, (255, 0, 0), -1)
                    cv2.circle(image, _eyes[1], 10, (255, 0, 0), -1)
                    cv2.polylines(image, numpy.array([_head_points]), True, (255, 255, 255), 10)
                    cv2.polylines(image, numpy.array([_eye_l]), True, (255, 255, 255), 5)
                    cv2.polylines(image, numpy.array([_eye_r]), True, (255, 255, 255), 5)

                if _left_hand_point:
                    cv2.line(image, _left_hand_point[0], _left_hand_point[1], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[5], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[9], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[13], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[17], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[1], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[5], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[9], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[13], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[0], _left_hand_point[17], (255, 255, 255), 3)

                    cv2.line(image, _left_hand_point[1], _left_hand_point[2], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[2], _left_hand_point[3], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[3], _left_hand_point[4], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[5], _left_hand_point[6], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[6], _left_hand_point[7], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[7], _left_hand_point[8], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[9], _left_hand_point[10], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[10], _left_hand_point[11], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[11], _left_hand_point[12], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[13], _left_hand_point[14], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[14], _left_hand_point[15], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[15], _left_hand_point[16], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[17], _left_hand_point[18], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[18], _left_hand_point[19], (0, 0, 0), 10)
                    cv2.line(image, _left_hand_point[19], _left_hand_point[20], (0, 0, 0), 10)

                    cv2.line(image, _left_hand_point[1], _left_hand_point[2], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[2], _left_hand_point[3], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[3], _left_hand_point[4], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[5], _left_hand_point[6], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[6], _left_hand_point[7], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[7], _left_hand_point[8], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[9], _left_hand_point[10], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[10], _left_hand_point[11], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[11], _left_hand_point[12], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[13], _left_hand_point[14], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[14], _left_hand_point[15], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[15], _left_hand_point[16], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[17], _left_hand_point[18], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[18], _left_hand_point[19], (255, 255, 255), 3)
                    cv2.line(image, _left_hand_point[19], _left_hand_point[20], (255, 255, 255), 3)

                    for i in range(21):
                        cv2.circle(image, _left_hand_point[i], 10, (255, 0, 0), -1)

                if _right_hand_point:
                    cv2.line(image, _right_hand_point[0], _right_hand_point[1], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[5], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[9], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[13], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[17], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[1], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[5], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[9], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[13], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[0], _right_hand_point[17], (255, 255, 255), 3)

                    cv2.line(image, _right_hand_point[1], _right_hand_point[2], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[2], _right_hand_point[3], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[3], _right_hand_point[4], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[5], _right_hand_point[6], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[6], _right_hand_point[7], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[7], _right_hand_point[8], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[9], _right_hand_point[10], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[10], _right_hand_point[11], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[11], _right_hand_point[12], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[13], _right_hand_point[14], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[14], _right_hand_point[15], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[15], _right_hand_point[16], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[17], _right_hand_point[18], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[18], _right_hand_point[19], (0, 0, 0), 10)
                    cv2.line(image, _right_hand_point[19], _right_hand_point[20], (0, 0, 0), 10)

                    cv2.line(image, _right_hand_point[1], _right_hand_point[2], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[2], _right_hand_point[3], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[3], _right_hand_point[4], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[5], _right_hand_point[6], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[6], _right_hand_point[7], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[7], _right_hand_point[8], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[9], _right_hand_point[10], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[10], _right_hand_point[11], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[11], _right_hand_point[12], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[13], _right_hand_point[14], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[14], _right_hand_point[15], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[15], _right_hand_point[16], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[17], _right_hand_point[18], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[18], _right_hand_point[19], (255, 255, 255), 3)
                    cv2.line(image, _right_hand_point[19], _right_hand_point[20], (255, 255, 255), 3)

                    for i in range(21):
                        cv2.circle(image, _right_hand_point[i], 10, (255, 0, 0), -1)




                resize_scale = 50
                image = cv2.resize(image, (16 * resize_scale, 9 * resize_scale))
                image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
                image = cv2.flip(image, 1)
                cv2.imshow('cum', image)
                cv2.waitKey(1)

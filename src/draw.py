import cv2
import numpy as np


# Display settings
SCREEN_SHAPE = (200, 100)
SCREEN_LOC = (37, 75)
SCREEN_BG_COLOR = (0, 1, 20)
SCREEN_PX_COLOR = (237, 255, 31)


def draw_screen(frame, text):
    frame[SCREEN_LOC[1]: SCREEN_LOC[1] + SCREEN_SHAPE[1], SCREEN_LOC[0]: SCREEN_LOC[0] + SCREEN_SHAPE[0]] = SCREEN_BG_COLOR
    c = [SCREEN_LOC[0] + SCREEN_SHAPE[0] // 2, SCREEN_LOC[1]]

    for t in text:
        c[1] += SCREEN_SHAPE[1] // 4
        font = 5
        scale = 1
        thickness = 1
        textsize = cv2.getTextSize(t, font, scale, thickness)[0]

        c_ = (c[0] - textsize[0] // 2, c[1] + textsize[1] // 2)
        frame = cv2.putText(frame, t, c_, font, scale, SCREEN_PX_COLOR, thickness)

    return frame


def draw_btns(frame, buttons):
    for button in buttons:
        c = button["center"]
        r = button["radius"]
        frame = cv2.circle(frame, c, r, button["color"], -1)
        frame = cv2.circle(frame, c, r, (0, 0, 0), 1)

        c = (c[0], c[1] + r + 15)
        font = 2
        scale = 0.75
        textsize = cv2.getTextSize(button["label"], font, scale, 2)[0]

        c = (c[0] - textsize[0] // 2, c[1] + textsize[1] // 2)
        frame = cv2.putText(frame, button["label"], c, font, scale, (0, 0, 0), 3)
    
    return frame


def draw_knob(frame, pot):
    frame = cv2.circle(frame, pot["center"], pot["radius"], pot["color"], -1)

    angle = (45 - 275 * pot["value"]) * np.pi / 180
    p = (
        pot["center"][0] - int(pot["radius"] * np.cos(angle)),
        pot["center"][1] + int(pot["radius"] * np.sin(angle))
    )

    frame = cv2.line(frame, pot["center"], p, (255, 255, 255), 3)
    frame = cv2.circle(frame, pot["center"], pot["radius"], pot["color"], 1)

    c = (pot["center"][0], pot["center"][1] + pot["radius"] + 15)
    font = 2
    scale = 0.65
    textsize = cv2.getTextSize(pot["label"], font, scale, 2)[0]

    c = (c[0] - textsize[0] // 2, c[1] + textsize[1] // 2)
    frame = cv2.putText(frame, pot["label"], c, font, scale, (0, 0, 0), 2)

    return frame


def draw_fs(frame, fs):
    frame = cv2.circle(frame, fs["center"], fs["radius"], fs["color"], -1)
    frame = cv2.circle(frame, fs["center"], fs["radius"], (0, 0, 0), 1)

    return frame

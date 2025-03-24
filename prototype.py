import cv2

from src.draw import *
from src.callbacks import Navigator


nav = Navigator()
knob_pressed = False
last_loc = None
pedal_base_img = cv2.imread("images/lata.png")

def __callback(*args):
    global knob_pressed
    global last_loc
    global pedal_base_img

    event, x, y = args[: 3]
    
    if event == 1:
        knob_pressed = nav.pressed(x, y) == 3

    elif event == 4:
        nav.release()
        knob_pressed = False

    elif event == 0 and knob_pressed:
        nav.knob_change(x > last_loc)

    else:
        return

    last_loc = x

    buttons, knob, fs = nav.get_elements()

    pedal_img = draw_screen(pedal_base_img, nav.get_text())
    pedal_img = draw_btns(pedal_img, buttons)
    pedal_img = draw_knob(pedal_img, knob)
    pedal_img = draw_fs(pedal_img, fs)

    cv2.imshow("Smart Pedal", pedal_img)
    cv2.waitKey(0)

text = ["Smart Pedal", "Iniciando", ""]
for i in range(30):
    char = "-/|\\-/|\\"[-i % 8]
    animation = "..."[:(i // 2) % 4]
    text[1] = animation + "Iniciando" + animation
    text[2] = (" " if char == "|" else "").join([char for _ in range(3)])

    buttons, knob, fs = nav.get_elements()

    pedal_img = draw_screen(pedal_base_img, text)
    pedal_img = draw_btns(pedal_img, buttons)
    pedal_img = draw_knob(pedal_img, knob)
    pedal_img = draw_fs(pedal_img, fs)

    cv2.imshow("Smart Pedal", pedal_img)
    cv2.waitKey(150)

cv2.namedWindow("Smart Pedal")
cv2.setMouseCallback("Smart Pedal", __callback)

__callback(1, 0, 0)

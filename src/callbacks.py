from src.menus import menu
from src.elements import *


DISPLAY_LENGTH = 14

class Navigator:
    def __init__(self):
        self.__menu = menu
        self.__buttons = (BTN_1, BTN_2, BTN_3)
        self.__knob = knob
        self.__fs = FOOTSWICH

        # LEVEL: 0- Effect selection; 1- Parameter selection; 2- Parameter setting
        self.__level = 0
        self.__index = [0, 0]
    
    def knob_change(self, up: bool = True):
        change = 0.01 if up else -0.01

        self.__knob["value"] += change

        if self.__knob["value"] < 0:
            self.__knob["value"] = 0
        elif self.__knob["value"] > 1:
            self.__knob["value"] = 1

        if self.__level == 2:
            effects = list(self.__menu.keys())
            parameters = list(self.__menu[effects[self.__index[0]]].keys())

            self.__menu[effects[self.__index[0]]][parameters[self.__index[1]]] = self.__knob["value"]
    
    def pressed(self, x, y):
        for i, btn in enumerate(self.__buttons):
            dist = (btn["center"][0] - x)**2 + (btn["center"][1] - y)**2

            if dist <= btn["radius"]**2:
                self.navigate(i)
                btn["color"] = BUTTON_PRESS_COLOR
                return i
            
        dist = (self.__knob["center"][0] - x)**2 + (self.__knob["center"][1] - y)**2
        if dist <= self.__knob["radius"]**2:
            return 3

    def release(self):
        for btn in self.__buttons:
            btn["color"] = BUTTON_COLOR

    def navigate(self, btn_index):
        if btn_index == 1: # OK
            if self.__level == 0:
                self.__level = 1
                self.__index[1] = 0
            
            elif self.__level == 1:
                self.__level = 2

            elif self.__level == 2:
                self.__level = 1
            
        elif btn_index == 0: # <
            if self.__level == 0:
                if self.__index[0] > 0:
                    self.__index[0] -= 1

            if self.__level == 1:
                if self.__index[1] > 0:
                    self.__index[1] -= 1
                else:
                    self.__level = 0
        
        elif btn_index == 2: # >
            if self.__level == 0:
                if self.__index[0] < len(self.__menu) -1:
                    self.__index[0] += 1

            if self.__level == 1:
                effects = list(self.__menu.keys())
                parameters = self.__menu[effects[self.__index[0]]].keys()
                if self.__index[1] < len(parameters) -1:
                    self.__index[1] += 1
                else:
                    self.__level = 0
    
    def get_text(self):
        if self.__level == 0:
            t = list(self.__menu.keys())[self.__index[0]]
            pre_len = (DISPLAY_LENGTH - len(t)) // 2
            pos_len = DISPLAY_LENGTH - len(t) - pre_len
            t = "".join([" " for _ in range(pre_len)]) + t + "".join([" " for _ in range(pos_len)])
            if self.__index[0] > 0:
                t = "<" + t[1:]
            if self.__index[0] < len(self.__menu) - 1:
                t = t[: -1] + ">"

            text = [
                "",
                t,
                ""
            ]
        
        elif self.__level == 1:
            effects = self.__menu[list(self.__menu.keys())[self.__index[0]]]
            t = list(effects.keys())[self.__index[1]]
            pre_len = (DISPLAY_LENGTH - len(t)) // 2
            pos_len = DISPLAY_LENGTH - len(t) - pre_len
            t = "".join([" " for _ in range(pre_len)]) + t + "".join([" " for _ in range(pos_len)])
            if self.__index[1] > 0:
                t = "<" + t[1:]
            if self.__index[1] < len(effects) - 1:
                t = t[: -1] + ">"

            text = [
                list(self.__menu.keys())[self.__index[0]],
                t,
                ""
            ]
        
        elif self.__level == 2:
            effects = self.__menu[list(self.__menu.keys())[self.__index[0]]]
            effect = list(effects.keys())[self.__index[1]]
            t = effect + f": {int(100 * self.__menu[list(self.__menu.keys())[self.__index[0]]][effect])}%"
            pre_len = 2
            pos_len = DISPLAY_LENGTH - len(t) - pre_len
            t = "".join([" " for _ in range(pre_len)]) + t + "".join([" " for _ in range(pos_len)])

            text = [
                list(self.__menu.keys())[self.__index[0]],
                effect,
                t
            ]

        return text

    def get_elements(self):
        return self.__buttons, self.__knob, self.__fs

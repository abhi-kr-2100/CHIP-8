from PySide6.QtCore import Qt

from PyCHIP8.PyCHIP8 import Key


SCALING_FACTOR = 10

KBD_TO_CHIP_8 = {
    Qt.Key.Key_X: Key.K0,
    Qt.Key.Key_1: Key.K1,
    Qt.Key.Key_2: Key.K2,
    Qt.Key.Key_3: Key.K3,
    Qt.Key.Key_Q: Key.K4,
    Qt.Key.Key_W: Key.K5,
    Qt.Key.Key_E: Key.K6,
    Qt.Key.Key_A: Key.K7,
    Qt.Key.Key_S: Key.K8,
    Qt.Key.Key_D: Key.K9,
    Qt.Key.Key_Z: Key.KA,
    Qt.Key.Key_C: Key.KB,
    Qt.Key.Key_4: Key.KC,
    Qt.Key.Key_R: Key.KD,
    Qt.Key.Key_F: Key.KE,
    Qt.Key.Key_V: Key.KF,
}

DEBUG_GO_FORWARD_KEY = Qt.Key.Key_Space
DEBUG_GO_BACK_KEY = Qt.Key.Key_Backspace

from PySide6.QtGui import QPixmap, QImage
from PySide6.QtWidgets import QGraphicsPixmapItem

from PyCHIP8.PyCHIP8 import MAX_NUM_INSTURCTIONS, INSTRUCTION_SIZE


def get_bytes(filename):
    with open(filename, 'rb') as rom_file:
        byte_str = rom_file.read()

    ins_bytes = [0] * (MAX_NUM_INSTURCTIONS * INSTRUCTION_SIZE)
    for i in range(len(byte_str)):
        ins_bytes[i] = int(byte_str[i])

    return ins_bytes


def get_graphics_from_frame_buffer(frame_buffer):
    width = len(frame_buffer)
    height = len(frame_buffer[0])

    img = QImage(width, height, QImage.Format.Format_Mono)
    for x, row in enumerate(frame_buffer):
        for y, pixel in enumerate(row):
            img.setPixel(x, y, 0 if pixel else 1)

    pixmap = QPixmap.fromImage(img)
    return QGraphicsPixmapItem(pixmap)

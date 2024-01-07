#!/usr/bin/env python3

from sys import argv, exit, stderr

from PySide6.QtCore import QTimer
from PySide6.QtGui import QImage, QPixmap
from PySide6.QtWidgets import QApplication, QGraphicsView, QGraphicsScene, QGraphicsItem, QGraphicsPixmapItem

from PyCHIP8.host.consts import KBD_TO_CHIP_8, SCALING_FACTOR
from PyCHIP8.host.helpers import get_bytes
from PyCHIP8.PyCHIP8 import (
    CHIP_8, MILLISECONDS_PER_REFRESH, INSTRUCTIONS_PER_REFRESH, TIMER_DECREMENTS_PER_REFRESH
)


def main():
    if len(argv) != 2:
        print(f"Usage: {argv[0]} file", file=stderr)
        exit(1)

    machine = CHIP_8()
    _, filename = argv
    ins_bytes = get_bytes(filename)
    machine.load_program_from_bytes(ins_bytes)

    app = CHIP8App(machine)
    return app.exec()


class CHIP8App(QApplication):
    def __init__(self, machine):
        super().__init__([])
        self.machine = machine

        self.scene = CHIP8FrameBuffer()
        self.screen = CHIP8Screen(self.scene, self.machine)
        self.screen.scale(SCALING_FACTOR, SCALING_FACTOR)

        self.refresh_timer = QTimer()
        self.refresh_timer.setInterval(MILLISECONDS_PER_REFRESH)
        self.refresh_timer.timeout.connect(self.refresh)
        self.refresh_timer.start()

        self.screen.show()

    def refresh(self):
        for _ in range(INSTRUCTIONS_PER_REFRESH):
            self.machine.run_one()
        self.machine.decrement_timers(TIMER_DECREMENTS_PER_REFRESH)
        self.scene.refresh(self.machine.frame_buffer)
        self.screen.update()


class CHIP8Screen(QGraphicsView):
    def __init__(self, scene, machine):
        super().__init__(scene)
        self.machine = machine

        width = len(self.machine.frame_buffer) * (SCALING_FACTOR + 1)
        height = len(self.machine.frame_buffer[0]) * (SCALING_FACTOR + 1)
        self.setFixedSize(width, height)

    def keyPressEvent(self, event):
        if (key := event.key()) in KBD_TO_CHIP_8:
            self.machine.keyboard.set_key_pressed(KBD_TO_CHIP_8[key])

    def keyReleaseEvent(self, event):
        if (key := event.key()) in KBD_TO_CHIP_8:
            self.machine.keyboard.set_key_released(KBD_TO_CHIP_8[key])


class CHIP8FrameBuffer(QGraphicsScene):
    def __init__(self):
        super().__init__()
        self.addText("Empty!")

    def refresh(self, new_frame_buffer):
        self.clear()

        width = len(new_frame_buffer)
        height = len(new_frame_buffer[0])

        img = QImage(width, height, QImage.Format.Format_Mono)
        for x, row in enumerate(new_frame_buffer):
            for y, pixel in enumerate(row):
                img.setPixel(x, y, 0 if pixel else 1)

        pixmap = QPixmap.fromImage(img)
        self.addItem(QGraphicsPixmapItem(pixmap))

        self.update()


if __name__ == '__main__':
    exit(main())

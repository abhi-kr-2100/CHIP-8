from PySide6.QtCore import QTimer
from PySide6.QtGui import QImage, QPixmap, QAction
from PySide6.QtWidgets import QApplication, QGraphicsView, QGraphicsScene, QGraphicsPixmapItem, QMainWindow, QToolBar, \
    QFileDialog

from PyCHIP8.host.consts import KBD_TO_CHIP_8, SCALING_FACTOR
from PyCHIP8.host.helpers import get_bytes

from PyCHIP8.PyCHIP8 import MILLISECONDS_PER_REFRESH, INSTRUCTIONS_PER_REFRESH, TIMER_DECREMENTS_PER_REFRESH


class CHIP8App(QApplication):
    def __init__(self, machine):
        super().__init__([])

        self.machine = machine

        width = len(self.machine.frame_buffer)
        height = len(self.machine.frame_buffer[0])
        self.screen = CHIP8GameScreen(width, height, SCALING_FACTOR)

        self.main_window = CHIP8MainWindow(self.screen, self.machine.keyboard, self.machine)

        self.refresh_timer = QTimer()
        self.refresh_timer.setInterval(MILLISECONDS_PER_REFRESH)
        self.refresh_timer.timeout.connect(self.refresh)
        self.refresh_timer.start()

        self.main_window.show()

    def refresh(self):
        for _ in range(INSTRUCTIONS_PER_REFRESH):
            self.machine.run_one()
        self.machine.decrement_timers(TIMER_DECREMENTS_PER_REFRESH)
        self.screen.refresh(self.machine.frame_buffer)


class CHIP8MainWindow(QMainWindow):
    def __init__(self, game_screen, keyboard, machine):
        super().__init__()

        self.game_screen = game_screen
        self.keyboard = keyboard
        self.machine = machine

        self.load_rom_action = QAction("Open file", self)
        self.load_rom_action.setStatusTip("Open and load a ROM.")
        self.load_rom_action.triggered.connect(self.load_rom)

        self.toolbar = CHIP8ToolBar([self.load_rom_action])
        self.addToolBar(self.toolbar)

        self.setCentralWidget(game_screen)
        self.setWindowTitle("CHIP-8")

    def load_rom(self):
        rom_name, _ = QFileDialog.getOpenFileName(self, "Open ROM", "")
        if rom_name:
            self.machine.load_program_from_bytes(get_bytes(rom_name))

    def keyPressEvent(self, event):
        if (key := event.key()) in KBD_TO_CHIP_8:
            self.keyboard.set_key_pressed(KBD_TO_CHIP_8[key])

    def keyReleaseEvent(self, event):
        if (key := event.key()) in KBD_TO_CHIP_8:
            self.keyboard.set_key_released(KBD_TO_CHIP_8[key])


class CHIP8ToolBar(QToolBar):
    def __init__(self, actions):
        super().__init__("CHIP-8 Toolbar")

        self.addActions(actions)


class CHIP8GameScreen(QGraphicsView):
    def __init__(self, width, height, scaling_factor):
        super().__init__()

        self.game_scene = CHIP8GameScreenScene()
        self.setScene(self.game_scene)

        self.setMinimumSize(width * (scaling_factor + 1), height * (scaling_factor + 1))
        self.scale(scaling_factor, scaling_factor)

    def refresh(self, frame_buffer):
        self.game_scene.refresh(frame_buffer)
        self.update()


class CHIP8GameScreenScene(QGraphicsScene):
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

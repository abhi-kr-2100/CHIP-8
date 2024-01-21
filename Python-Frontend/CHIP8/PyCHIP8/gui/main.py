from PySide6.QtCore import QTimer
from PySide6.QtGui import QImage, QPixmap, QAction
from PySide6.QtWidgets import QApplication, QGraphicsView, QGraphicsScene, QGraphicsPixmapItem, QMainWindow, QToolBar, \
    QFileDialog

from PyCHIP8.PyCHIP8 import MILLISECONDS_PER_REFRESH, INSTRUCTIONS_PER_REFRESH, TIMER_DECREMENTS_PER_REFRESH, Debugger

from PyCHIP8.host.consts import KBD_TO_CHIP_8, SCALING_FACTOR, DEBUG_GO_FORWARD_KEY, DEBUG_GO_BACK_KEY
from PyCHIP8.host.helpers import get_bytes

from PyCHIP8.gui.debugger.registers import RegistersView
from PyCHIP8.gui.debugger.memory import MemoryView


class CHIP8App(QApplication):
    def __init__(self, machine):
        super().__init__([])

        self.machine = machine
        self.debugger = Debugger(machine)
        self.debug_mode = False

        width = len(self.machine.frame_buffer)
        height = len(self.machine.frame_buffer[0])
        self.screen = CHIP8GameScreen(width, height, SCALING_FACTOR)

        self.load_rom_action = QAction("Open file", self)
        self.load_rom_action.setStatusTip("Open and load a ROM.")
        self.load_rom_action.triggered.connect(self.load_rom)

        self.toggle_debug_mode_action = QAction("Toggle Debug Mode", self)
        self.toggle_debug_mode_action.setStatusTip("Toggle Debug Mode")
        self.toggle_debug_mode_action.triggered.connect(self.toggle_debug_mode)

        self.main_window = CHIP8MainWindow(
            self.screen, self.machine.keyboard, self.machine, self.debugger,
            [self.load_rom_action, self.toggle_debug_mode_action], self.debug_mode
        )

        self.refresh_timer = QTimer()
        self.refresh_timer.setInterval(MILLISECONDS_PER_REFRESH)
        self.refresh_timer.timeout.connect(self.refresh)
        if not self.debug_mode:
            self.refresh_timer.start()

        self.main_window.show()

        self.extra_debug_windows = [RegistersView(self.debugger), MemoryView(self.debugger)]
        if self.debug_mode:
            for debug_window in self.extra_debug_windows:
                debug_window.show()

    def refresh(self):
        for _ in range(INSTRUCTIONS_PER_REFRESH):
            # always run the debugger even in non-debug mode to store previous states
            self.debugger.run_one_without_callback()
        self.machine.decrement_timers(TIMER_DECREMENTS_PER_REFRESH)
        self.screen.refresh(self.machine.frame_buffer)

    def load_rom(self):
        rom_name, _ = QFileDialog.getOpenFileName(self.main_window, "Open ROM", "")
        if rom_name:
            self.machine.load_program_from_bytes(get_bytes(rom_name))

    def toggle_debug_mode(self):
        if self.debug_mode:
            self.debug_mode = False
            self.main_window.set_debug_mode(False)
            self.refresh_timer.start()
        else:
            self.debug_mode = True
            self.main_window.set_debug_mode(True)
            self.refresh_timer.stop()

        for debug_window in self.extra_debug_windows:
            debug_window.setVisible(self.debug_mode)


class CHIP8MainWindow(QMainWindow):
    def __init__(self, game_screen, keyboard, machine, debugger, actions, debug_mode):
        super().__init__()

        self.game_screen = game_screen
        self.keyboard = keyboard
        self.machine = machine

        self.debug_mode = debug_mode
        self.debugger = debugger
        # Number of instructions executed in debug mode since last refresh. Since time doesn't flow in debug mode,
        # we use this to determine how much time has passed.
        self.ins_executed_since_refresh = 0

        self.toolbar = CHIP8ToolBar(actions)
        self.addToolBar(self.toolbar)

        self.setCentralWidget(game_screen)
        self.setWindowTitle("CHIP-8")

    def set_debug_mode(self, mode):
        self.debug_mode = mode

    def debugger_go_forward(self):
        self.debugger.run_one()

        # Refresh screen as soon as an instruction executes to reflect the changes that instruction might have caused
        self.game_screen.refresh(self.machine.frame_buffer)

        self.ins_executed_since_refresh += 1
        self.ins_executed_since_refresh %= INSTRUCTIONS_PER_REFRESH
        if self.ins_executed_since_refresh == 0:
            self.machine.decrement_timers(TIMER_DECREMENTS_PER_REFRESH)

    def debugger_go_back(self):
        self.debugger.go_back_one()

        # Refresh screen as soon as an instruction executes to reflect the changes that instruction might have caused
        self.game_screen.refresh(self.machine.frame_buffer)

        self.ins_executed_since_refresh -= 1
        self.ins_executed_since_refresh %= INSTRUCTIONS_PER_REFRESH

    def keyPressEvent(self, event):
        key = event.key()
        if key in KBD_TO_CHIP_8:
            self.keyboard.set_key_pressed(KBD_TO_CHIP_8[key])
        if self.debug_mode:
            if key == DEBUG_GO_FORWARD_KEY:
                self.debugger_go_forward()
            elif key == DEBUG_GO_BACK_KEY:
                self.debugger_go_back()

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

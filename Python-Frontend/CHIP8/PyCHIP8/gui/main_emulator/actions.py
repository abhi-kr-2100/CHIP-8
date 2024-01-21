from PySide6.QtGui import QAction

from PyCHIP8.host.consts import ExecutionMode


class LoadROMAction(QAction):
    def __init__(self, parent):
        super().__init__("Open File", parent)
        self.setStatusTip("Open and load a ROM.")
        self.triggered.connect(self.parent().load_rom)


class ToggleBreakModeAction(QAction):
    def __init__(self, parent):
        super().__init__("", parent)

        self.refresh_name()
        self.setStatusTip("Toggle Break Mode.")
        self.triggered.connect(self.trigger_action)

    def trigger_action(self):
        self.parent().toggle_break_mode()
        self.refresh_name()

    def refresh_name(self):
        self.setText("Pause" if self.parent().execution_mode != ExecutionMode.BREAK else "Resume")


class ToggleDebugMode(QAction):
    def __init__(self, parent):
        super().__init__("", parent)

        self.refresh_name()
        self.setStatusTip("Toggle Debug Mode.")
        self.triggered.connect(self.trigger_action)

    def trigger_action(self):
        self.parent().toggle_debug_mode()
        self.refresh_name()

    def refresh_name(self):
        self.setText("Debug" if self.parent().execution_mode == ExecutionMode.NORMAL else "Turn off debugging")

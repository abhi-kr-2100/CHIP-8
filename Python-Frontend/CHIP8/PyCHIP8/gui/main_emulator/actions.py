from PySide6.QtGui import QAction


class LoadROMAction(QAction):
    def __init__(self, parent):
        super().__init__("Open File", parent)
        self.setStatusTip("Open and load a ROM.")
        self.triggered.connect(self.parent().load_rom)


class ToggleDebugModeAction(QAction):
    def __init__(self, parent):
        super().__init__("Toggle Debug Mode", parent)
        self.setStatusTip("Toggle Debug Mode.")
        self.triggered.connect(self.parent().toggle_debug_mode)

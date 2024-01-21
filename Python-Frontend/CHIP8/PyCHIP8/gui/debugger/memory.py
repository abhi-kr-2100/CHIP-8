from PySide6.QtCore import QStringListModel
from PySide6.QtWidgets import QListView

from PyCHIP8.emulator import debugger


class MemoryModel(QStringListModel):
    def __init__(self):
        super().__init__()

        self.refresh()  # first refresh so that something is visible
        debugger.on_exec(lambda: self.refresh())

    def refresh(self):
        items = [hex(item) for item in debugger.memory]
        self.setStringList(items)


class MemoryView(QListView):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Memory")
        self.setModel(MemoryModel())

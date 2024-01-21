from PySide6.QtCore import QStringListModel
from PySide6.QtWidgets import QListView


class MemoryModel(QStringListModel):
    def __init__(self, debugger):
        super().__init__()

        self.debugger = debugger
        self.refresh()  # first refresh so that something is visible
        self.debugger.on_exec(lambda: self.refresh())

    def refresh(self):
        items = [hex(item) for item in self.debugger.memory]
        self.setStringList(items)


class MemoryView(QListView):
    def __init__(self, debugger):
        super().__init__()

        self.setWindowTitle("Memory")
        self.setModel(MemoryModel(debugger))

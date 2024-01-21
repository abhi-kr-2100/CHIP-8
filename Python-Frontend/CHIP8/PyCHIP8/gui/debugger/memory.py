from PySide6.QtCore import QStringListModel
from PySide6.QtWidgets import QListView

from PyCHIP8.emulator import debugger
from PyCHIP8.host.helpers import affects_memory


class MemoryModel(QStringListModel):
    def __init__(self):
        super().__init__()

        debugger.on_exec(self.refresh_if_needed)

    def refresh_if_needed(self, _, instruction):
        if not affects_memory(instruction):
            return
        items = [hex(item) for item in debugger.memory]
        self.setStringList(items)


class MemoryView(QListView):
    def __init__(self):
        super().__init__()

        self.setUniformItemSizes(True)

        self.setWindowTitle("Memory")
        self.setModel(MemoryModel())

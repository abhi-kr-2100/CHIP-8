from PySide6.QtCore import QStringListModel
from PySide6.QtWidgets import QListView


class RegistersModel(QStringListModel):
    def __init__(self, debugger):
        super().__init__()

        self.debugger = debugger
        self.refresh()  # first refresh so that something is visible
        self.debugger.on_exec(lambda: self.refresh())

    def refresh(self):
        items = [hex(item) for item in self.debugger.registers + [self.debugger.index_register]]
        self.setStringList(items)


class RegistersView(QListView):
    def __init__(self, debugger):
        super().__init__()

        self.setWindowTitle("Registers")
        self.setModel(RegistersModel(debugger))

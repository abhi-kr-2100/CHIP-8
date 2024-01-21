from PySide6.QtCore import QStringListModel
from PySide6.QtWidgets import QListView

from PyCHIP8.emulator import debugger
from PyCHIP8.host.helpers import affects_registers


class RegistersModel(QStringListModel):
    def __init__(self):
        super().__init__()

        debugger.on_exec(self.refresh_if_needed)

    def refresh_if_needed(self, _, ins):
        if not affects_registers(ins):
            return
        items = [hex(item) for item in debugger.registers + [debugger.index_register]]
        self.setStringList(items)


class RegistersView(QListView):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Registers")
        self.setModel(RegistersModel())

from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel


class RegistersView(QWidget):
    def __init__(self, debugger):
        super().__init__()

        self.debugger = debugger

        self.setWindowTitle("Registers View")

        self.layout = QVBoxLayout()

        self.index_register_label = QLabel()
        self.layout.addWidget(self.index_register_label)

        self.registers_label = QLabel()
        self.layout.addWidget(self.registers_label)

        self.setLayout(self.layout)

    def refresh(self):
        self.index_register_label.setText(f"Index Register: {self.debugger.index_register}")
        self.registers_label.setText(
            " ".join(f"{self.debugger.get_register(i):#04x}" for i in range(0x0, 0xF + 1)))

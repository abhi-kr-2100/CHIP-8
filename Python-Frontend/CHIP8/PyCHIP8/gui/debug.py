from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QScrollArea, QLayout


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


class MemoryView(QWidget):
    def __init__(self, debugger):
        super().__init__()

        self.debugger = debugger

        self.setWindowTitle("Memory View")

        self.container = QWidget()
        self.container.setMinimumSize(50, 50)
        self.container_layout = QVBoxLayout(self.container)
        self.container_layout.setSizeConstraint(QLayout.SizeConstraint.SetMinAndMaxSize)

        self.memory_labels = [QLabel() for _ in debugger.memory]
        for label in self.memory_labels:
            self.container_layout.addWidget(label)

        self.scroll_area = QScrollArea()
        self.scroll_area.setWidget(self.container)

        self.layout = QVBoxLayout(self)
        self.layout.addWidget(self.scroll_area)

    def refresh(self):
        for label, memory in zip(self.memory_labels, self.debugger.memory):
            label.setText(f"{memory:#04x}")

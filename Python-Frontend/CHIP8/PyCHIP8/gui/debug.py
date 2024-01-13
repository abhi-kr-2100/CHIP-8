from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QScrollArea, QLayout
from PySide6.QtGui import QFont


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

        self.highlight_font = QFont()
        self.highlight_font.setBold(True)

        self.normal_font = QFont()

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
        for i, (label, memory) in enumerate(zip(self.memory_labels, self.debugger.memory)):
            label.setText(f"{i:#04}: {memory:#04x}" + (" ← [PC]" if i == self.debugger.pc else ""))
            if i == self.debugger.pc:
                label.setFont(self.highlight_font)
            else:
                label.setFont(self.normal_font)

#!/usr/bin/env python3

from sys import argv, exit, stderr

from PyCHIP8.emulator import machine
from PyCHIP8.gui.main_emulator.main import CHIP8App
from PyCHIP8.host.helpers import get_bytes


def main():
    if len(argv) > 2:
        print(f"Usage: {argv[0]} [file]", file=stderr)
        exit(1)

    if len(argv) == 2:
        machine.load_program_from_bytes(get_bytes(argv[1]))

    app = CHIP8App()
    return app.exec()


if __name__ == '__main__':
    exit(main())

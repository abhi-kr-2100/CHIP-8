#!/usr/bin/env python3

from sys import argv, exit, stderr

from PyCHIP8.PyCHIP8 import CHIP_8

from PyCHIP8.gui.main import CHIP8App
from PyCHIP8.host.helpers import get_bytes


def main():
    if len(argv) > 2:
        print(f"Usage: {argv[0]} [file]", file=stderr)
        exit(1)

    machine = CHIP_8()
    if len(argv) == 2:
        machine.load_program_from_bytes(get_bytes(argv[1]))

    app = CHIP8App(machine)
    return app.exec()


if __name__ == '__main__':
    exit(main())

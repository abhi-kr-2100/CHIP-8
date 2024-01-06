#!/usr/bin/env python3


from sys import argv, exit, stderr
from time import time_ns

import pygame

from PyCHIP8.host.consts import SCALING_FACTOR, KBD_TO_CHIP_8
from PyCHIP8.host.helpers import draw_frame_buffer_on_surface, get_bytes
from PyCHIP8.PyCHIP8 import (
    CHIP_8, MILLISECONDS_PER_REFRESH,
    INSTRUCTIONS_PER_REFRESH, TIMER_DECREMENTS_PER_REFRESH
)


def main():
    if len(argv) != 2:
        print(f"Usage: {argv[0]} file", file=stderr)
        exit(1)

    machine = CHIP_8()
    _, filename = argv
    ins_bytes = get_bytes(filename)
    machine.load_program_from_bytes(ins_bytes)

    width = len(machine.frame_buffer)
    height = len(machine.frame_buffer[0])

    pygame.init()
    pygame.display.set_caption("CHIP-8")
    screen = pygame.display.set_mode((width * SCALING_FACTOR, height * SCALING_FACTOR))

    canvas = pygame.surface.Surface((width, height))

    last_limiter_check_time = time_ns()
    while True:
        event = pygame.event.poll()
        match event.type:
            case pygame.QUIT:
                break
            case pygame.KEYDOWN if event.scancode in KBD_TO_CHIP_8:
                machine.keyboard.set_key_pressed(KBD_TO_CHIP_8[event.scancode])
            case pygame.KEYUP if event.scancode in KBD_TO_CHIP_8:
                machine.keyboard.set_key_released(KBD_TO_CHIP_8[event.scancode])

        curr_time = time_ns()
        millis_elapsed = int((curr_time - last_limiter_check_time) / 1e6)

        if millis_elapsed < MILLISECONDS_PER_REFRESH:
            continue
        last_limiter_check_time = curr_time

        refreshes_elapsed = int(millis_elapsed / MILLISECONDS_PER_REFRESH)

        ins_elapsed = refreshes_elapsed * INSTRUCTIONS_PER_REFRESH
        for _ in range(ins_elapsed):
            machine.run_one()

        timer_decrements_elapsed = refreshes_elapsed * TIMER_DECREMENTS_PER_REFRESH
        machine.decrement_timers(timer_decrements_elapsed)

        draw_frame_buffer_on_surface(canvas, machine.frame_buffer)
        screen.blit(pygame.transform.scale(canvas, screen.get_size()), (0, 0))
        pygame.display.flip()


if __name__ == '__main__':
    main()

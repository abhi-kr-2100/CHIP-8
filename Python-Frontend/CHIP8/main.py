#!/usr/bin/env python3


from sys import argv, exit, stderr
from time import time_ns

import pygame

from PyCHIP8.PyCHIP8 import (
    CHIP_8, MAX_NUM_INSTURCTIONS, INSTRUCTION_SIZE, Key,
    MILLISECONDS_PER_REFRESH, INSTRUCTIONS_PER_REFRESH, TIMER_DECREMENTS_PER_REFRESH
)

SCALING_FACTOR = 10

KBD_TO_CHIP_8 = {
    pygame.KSCAN_X: Key.K0,
    pygame.KSCAN_1: Key.K1,
    pygame.KSCAN_2: Key.K2,
    pygame.KSCAN_3: Key.K3,
    pygame.KSCAN_Q: Key.K4,
    pygame.KSCAN_W: Key.K5,
    pygame.KSCAN_E: Key.K6,
    pygame.KSCAN_A: Key.K7,
    pygame.KSCAN_S: Key.K8,
    pygame.KSCAN_D: Key.K9,
    pygame.KSCAN_Z: Key.KA,
    pygame.KSCAN_C: Key.KB,
    pygame.KSCAN_4: Key.KC,
    pygame.KSCAN_R: Key.KD,
    pygame.KSCAN_F: Key.KE,
    pygame.KSCAN_V: Key.KF,
}


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


def draw_frame_buffer_on_surface(screen, frame_buffer):
    screen.fill((255, 255, 255))
    for x in range(len(frame_buffer)):
        for y in range(len(frame_buffer[0])):
            is_black = frame_buffer[x][y] != 0
            if is_black:
                screen.set_at((x, y), (0, 0, 0))


def get_bytes(filename):
    with open(filename, 'rb') as rom_file:
        byte_str = rom_file.read()

    ins_bytes = [0] * (MAX_NUM_INSTURCTIONS * INSTRUCTION_SIZE)
    for i in range(len(byte_str)):
        ins_bytes[i] = int(byte_str[i])

    return ins_bytes


if __name__ == '__main__':
    main()

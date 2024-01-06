import pygame

from PyCHIP8.PyCHIP8 import Key


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

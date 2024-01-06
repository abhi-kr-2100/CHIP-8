from PyCHIP8.PyCHIP8 import MAX_NUM_INSTURCTIONS, INSTRUCTION_SIZE


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

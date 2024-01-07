from PyCHIP8.PyCHIP8 import MAX_NUM_INSTURCTIONS, INSTRUCTION_SIZE


def get_bytes(filename):
    with open(filename, 'rb') as rom_file:
        byte_str = rom_file.read()

    ins_bytes = [0] * (MAX_NUM_INSTURCTIONS * INSTRUCTION_SIZE)
    for i in range(len(byte_str)):
        ins_bytes[i] = int(byte_str[i])

    return ins_bytes

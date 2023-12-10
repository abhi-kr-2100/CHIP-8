#pragma once

constexpr auto BITS_PER_BYTE = 8;

constexpr auto MEMORY_SIZE = 4096 /* bytes */;
constexpr auto STACK_SIZE = 32 /* bytes */;
constexpr auto STACK_ENTRY_SIZE = 2 /* bytes */;

constexpr auto NUM_REGISTERS = 16;

constexpr auto FRAME_BUFFER_WIDTH = 64 /* pixels */;
constexpr auto FRAME_BUFFER_HEIGHT = 32 /* pixels */;

constexpr auto FONT_DATA_START_LOCATION = 0x50;

constexpr auto INSTRUCTION_SIZE = 2 /* bytes */;
constexpr auto PROGRAM_DATA_START_LOCATION = 0x200;
constexpr auto MAX_NUM_INSTRUCTIONS = (MEMORY_SIZE - PROGRAM_DATA_START_LOCATION) / INSTRUCTION_SIZE;
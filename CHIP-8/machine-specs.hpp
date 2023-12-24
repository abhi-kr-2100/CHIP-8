#pragma once

#include <cmath>

constexpr auto BITS_PER_BYTE = 8;
constexpr auto BITS_PER_NIBBLE = 4;
constexpr auto NIBBLES_PER_BYTE = BITS_PER_BYTE / BITS_PER_NIBBLE;

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

constexpr auto MILLISECONDS_PER_SECOND = 1000;

constexpr auto EXECUTION_SPEED = 540; /* instructions per second */
const auto MILLISECONDS_PER_INSTRUCTION = std::ceil(double(MILLISECONDS_PER_SECOND) / EXECUTION_SPEED);
constexpr auto SCREEN_REFRESHES_PER_SECOND = 60; /* FPS */
constexpr auto MILLISECONDS_PER_REFRESH = MILLISECONDS_PER_SECOND / SCREEN_REFRESHES_PER_SECOND;
constexpr auto INSTRUCTIONS_PER_REFRESH = EXECUTION_SPEED / SCREEN_REFRESHES_PER_SECOND;
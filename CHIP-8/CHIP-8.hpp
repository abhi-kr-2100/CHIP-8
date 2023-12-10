#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <map>

#include "font-data.hpp"

using byte = std::uint8_t;
using double_byte = std::uint16_t;
using instruction_t = double_byte;

constexpr auto bits_per_byte = 8;

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

class CHIP_8
{
public:
	void load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program);
	void run();

	CHIP_8();
private:
	std::array<byte, MEMORY_SIZE> memory;
	std::array<byte, NUM_REGISTERS> registers;
	std::array<double_byte, STACK_SIZE / STACK_ENTRY_SIZE> stack;

	std::array<std::array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	std::map<byte, std::function<void(byte, byte, byte, byte, double_byte)>>
		executors;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
};
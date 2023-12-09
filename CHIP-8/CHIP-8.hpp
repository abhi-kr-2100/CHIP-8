#pragma once

#include <cstdint>
#include <array>

#include "font-data.hpp"

constexpr auto MEMORY_SIZE = 4096 /* bytes */;
constexpr auto STACK_SIZE = 32 /* bytes */;
constexpr auto STACK_POINTER_SIZE = 2 /* bytes */;

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
	void load_program(const std::array<std::uint16_t, MAX_NUM_INSTRUCTIONS>& program);
	void run();

	CHIP_8()
	{
		load_fonts(FONT_DATA_START_LOCATION, FONT_DATA);
	}

private:
	std::array<std::uint8_t, MEMORY_SIZE> memory;
	std::array<std::uint8_t, NUM_REGISTERS> registers;
	std::array<std::uint16_t, STACK_SIZE / STACK_POINTER_SIZE> stack;

	std::array<std::array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> frame_buffer;

	std::uint16_t pc;
	std::uint16_t index_register;
	std::uint8_t stack_pointer;

	std::uint8_t delay_timer;
	std::uint8_t sound_timer;

	void load_fonts(std::uint16_t start_location, const decltype(FONT_DATA)& font_data);
};
#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <map>

#include "data-types.hpp"
#include "font-data.hpp"
#include "machine-specs.hpp"

class CHIP_8
{
public:
	void load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program);
	void run();

	CHIP_8();

	bool get_frame_buffer_pixel(size_t x, size_t y);

	// machine instructions
	void clear_screen(byte, byte, byte, byte, double_byte);
	void jump(byte, byte, byte, byte, double_byte NNN);
	void set_register(byte X, byte, byte, byte NN, double_byte);
	void add(byte X, byte, byte, byte NN, double_byte);
	void set_index_register(byte, byte, byte, byte, double_byte NNN);
	void draw(byte X, byte Y, byte N, byte, double_byte);
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

	std::map<byte, void(CHIP_8::*)(byte, byte, byte, byte, double_byte)> executors;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
};
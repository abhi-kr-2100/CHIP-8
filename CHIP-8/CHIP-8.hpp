#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <map>

#include <ftxui/screen/screen.hpp>

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
	void set_frame_buffer_pixel(size_t x, size_t y, bool val);

	// CHIP-8 op codes are two bytes (or 4 nibbles). The category of the op code
	// is determined by the first nibble, while the remaining 3 nibbles may be
	// grouped in various ways depending on the particular instruction.
	// We dedicate one method each to each category of op code. We group the
	// remaining nibbles in the following way:
	// If 0xABCD is the opcode, A, the first nibble, is the category.
	// B is the second nibble (stored in a byte since byte is the smallest
	// addressable unit).
	// C is the third nibble.
	// D is the fourth nibble.
	// CD is the second byte.
	// BCD is the second nibble along with the second byte.
	void ins_0(byte, byte, byte, byte, double_byte);
	void ins_1(byte, byte, byte, byte, double_byte);
	void ins_2(byte, byte, byte, byte, double_byte);
	void ins_3(byte, byte, byte, byte, double_byte);
	void ins_4(byte, byte, byte, byte, double_byte);
	void ins_5(byte, byte, byte, byte, double_byte);
	void ins_6(byte, byte, byte, byte, double_byte);
	void ins_7(byte, byte, byte, byte, double_byte);
	void ins_8(byte, byte, byte, byte, double_byte);
	void ins_9(byte, byte, byte, byte, double_byte);
	void ins_A(byte, byte, byte, byte, double_byte);
	void ins_B(byte, byte, byte, byte, double_byte);
	void ins_C(byte, byte, byte, byte, double_byte);
	void ins_D(byte, byte, byte, byte, double_byte);
	void ins_E(byte, byte, byte, byte, double_byte);
	void ins_F(byte, byte, byte, byte, double_byte);

	// named instructions -- helper methods used by the above
	void clear_screen();
	void return_();
	void jump(double_byte NNN);
	void set_register(byte X, byte NN);
	void add(byte X, byte NN);
	void set_index_register(double_byte NNN);
	void draw(byte X, byte Y, byte N);
private:
	std::array<byte, MEMORY_SIZE> memory;
	std::array<byte, NUM_REGISTERS> registers;
	std::array<double_byte, STACK_SIZE / STACK_ENTRY_SIZE> stack;

	ftxui::Screen frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	std::map<byte, void(CHIP_8::*)(byte, byte, byte, byte, double_byte)> executors;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
};
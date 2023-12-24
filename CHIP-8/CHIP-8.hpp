#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <map>

#include "data-types.hpp"
#include "font-data.hpp"
#include "machine-specs.hpp"

class Executor;

class CHIP_8
{
public:
	void load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program);
	bool run_one();

	byte get_pixel_at(size_t x, size_t y) const;
	void decrement_timers(byte times);

	CHIP_8();
	~CHIP_8();

	/**
	 * A CHIP-8 instruction is 2 bytes (or 4 nibbles) long, and can be
	 * represented using 4 hexadecimal digits (one for each nibble).
	 *
	 * The leftmost digit is the category which determines how the
	 * remaining 3 digits are interpreted.
	 *
	 * The various interpretations of the other nibbles are as follows:
	 * - X: the 2nd nibble
	 * - Y: the 3rd nibble
	 * - N: the 4th nibble
	 * - NN: the 2nd byte
	 * - NNN: the 2nd, 3rd, and the 4th nibbles
	 */
	static struct Instruction
	{
		static struct Instruction_payload
		{
			byte X;
			byte Y;
			byte N;
			byte NN;
			double_byte NNN;
		};

		instruction_t raw_instruction;
		byte category;
		Instruction_payload payload;
	};
private:
	std::array<byte, MEMORY_SIZE> memory;
	std::array<byte, NUM_REGISTERS> registers;
	std::array<double_byte, STACK_SIZE / STACK_ENTRY_SIZE> stack;

	std::array<std::array<byte, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
	Instruction get_current_instruction() const;

	Executor* executor;

	friend class Executor;
};
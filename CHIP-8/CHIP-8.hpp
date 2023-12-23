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
	bool run_one();

	bool get_pixel_at(size_t x, size_t y) const;

	CHIP_8();

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

	void ins_0(const Instruction::Instruction_payload& payload);
	void ins_1(const Instruction::Instruction_payload& payload);
	void ins_2(const Instruction::Instruction_payload& payload);
	void ins_3(const Instruction::Instruction_payload& payload);
	void ins_4(const Instruction::Instruction_payload& payload);
	void ins_5(const Instruction::Instruction_payload& payload);
	void ins_6(const Instruction::Instruction_payload& payload);
	void ins_7(const Instruction::Instruction_payload& payload);
	void ins_8(const Instruction::Instruction_payload& payload);
	void ins_9(const Instruction::Instruction_payload& payload);
	void ins_A(const Instruction::Instruction_payload& payload);
	void ins_B(const Instruction::Instruction_payload& payload);
	void ins_C(const Instruction::Instruction_payload& payload);
	void ins_D(const Instruction::Instruction_payload& payload);
	void ins_E(const Instruction::Instruction_payload& payload);
	void ins_F(const Instruction::Instruction_payload& payload);

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

	std::array<std::array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	std::map<byte, void(CHIP_8::*)(const Instruction::Instruction_payload&)> executors;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
	Instruction get_current_instruction() const;
};
#include <cassert>

#include "CHIP-8.hpp"
#include "executors-data.hpp"
#include "helpers.hpp"

CHIP_8::CHIP_8()
	: memory{}, registers{}, stack{}, frame_buffer{},
	  pc{PROGRAM_DATA_START_LOCATION}, index_register{}, stack_pointer{},
	  delay_timer{}, sound_timer{}
{
	load_fonts(FONT_DATA_START_LOCATION, FONT_DATA);
	
	executors[0x0] = clear_screen;
	executors[0x1] = jump;
	executors[0x6] = set_register;
	executors[0x7] = add;
	executors[0xA] = set_index_register;
	executors[0xD] = draw;
}

void CHIP_8::load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program)
{
	for (size_t i = 0, j = 0; i < MAX_NUM_INSTRUCTIONS; ++i, j += 2)
	{
		assert(i + PROGRAM_DATA_START_LOCATION < MEMORY_SIZE);
		memory[j + PROGRAM_DATA_START_LOCATION] = program[i] >> (sizeof(byte) * BITS_PER_BYTE);
		memory[j + 1 + PROGRAM_DATA_START_LOCATION] = program[i] & 0x00FF;
	}
}

/**
 * A CHIP-8 instruction is 2 bytes long. The first half-byte ("nibble") of the opcode
 * is the category of the instruction. The interpretation of the rest of the nibbles
 * depends on the opcode.
 * 
 * The various interpretations of the other nibbles are as follows:
 * - X: (the 2nd nibble): used to look up one of the 16 registers.
 * - Y: (the 3rd nibble): also used to look up one of the 16 registers.
 * - N: (the 4th nibble): a 4-bit number.
 * - NN: (the 2nd byte): an 8-bit number.
 * - NNN: (the 2nd, 3rd, and the 4th nibble): a 12-bit immediate memory address.
 */
void CHIP_8::run()
{
	assert(pc < MEMORY_SIZE);
	const instruction_t curr_instruction = memory[pc] << (sizeof(byte) * BITS_PER_BYTE) | memory[pc + 1];
	pc += 2;

	if (curr_instruction == 0)
	{
		return;
	}

	const byte category = get_nibbles_in_range(curr_instruction, 0, 0);

	// although the nibble/s required depends on the particular opcode, we decode
	// everything at one place for simplicity
	const byte X = get_nibbles_in_range(curr_instruction, 1, 1);
	const byte Y = get_nibbles_in_range(curr_instruction, 2, 2);
	const byte N = get_nibbles_in_range(curr_instruction, 3, 3);
	const byte NN = get_nibbles_in_range(curr_instruction, 2, 3);
	const double_byte NNN = get_nibbles_in_range(curr_instruction, 1, 3);

	assert(executors.find(category) != executors.end());
	executors[category](X, Y, N, NN, NNN, *this);

	run();
}

void CHIP_8::load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data)
{
	assert(start_location < MEMORY_SIZE);
	assert(FONT_NUM_CHARS == font_data.size());
	assert(FONT_CHAR_SIZE == font_data[0].size());
	
	for (size_t i = 0; i < FONT_NUM_CHARS; ++i)
	{
		for (size_t j = 0; j < FONT_CHAR_SIZE; ++j)
		{
			assert(FONT_DATA_START_LOCATION + FONT_CHAR_SIZE * i + j < MEMORY_SIZE);
			memory[FONT_DATA_START_LOCATION + FONT_CHAR_SIZE * i + j] = font_data[i][j];
		}
	}
}
#include <cassert>

#include "CHIP-8.hpp"
#include "helpers.hpp"

CHIP_8::CHIP_8()
{
	for (auto& m : memory)
	{
		m = 0;
	}
	for (auto& r : registers)
	{
		r = 0;
	}
	for (auto& s : stack)
	{
		s = 0;
	}
	for (auto& r : frame_buffer)
	{
		for (auto& c : r)
		{
			c = 0;
		}
	}

	load_fonts(FONT_DATA_START_LOCATION, FONT_DATA);

	executors[0x0] = &CHIP_8::clear_screen;
	executors[0x1] = &CHIP_8::jump;
	executors[0x6] = &CHIP_8::set_register;
	executors[0x7] = &CHIP_8::add;
	executors[0xA] = &CHIP_8::set_index_register;
	executors[0xD] = &CHIP_8::draw;
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
	(this->*executors[category])(X, Y, N, NN, NNN);

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

void CHIP_8::clear_screen(byte, byte, byte, byte NN, double_byte)
{
	for (size_t x = 0, width = frame_buffer.size(); x < width; ++x)
	{
		for (size_t y = 0, height = frame_buffer[x].size(); y < height; ++y)
		{
			frame_buffer[x][y] = false;
		}
	}
}

void CHIP_8::jump(byte, byte, byte, byte N, double_byte NNN)
{
	assert(NNN < memory.size());
	pc = NNN;
}

void CHIP_8::set_register(byte X, byte, byte N, byte NN, double_byte)
{
	assert(X < registers.size());
	registers[X] = NN;
}

void CHIP_8::add(byte X, byte, byte N, byte NN, double_byte)
{
	assert(X < registers.size());
	registers[X] += NN;
}

void CHIP_8::set_index_register(byte, byte, byte, byte NN, double_byte NNN)
{
	index_register = NNN;
}

void CHIP_8::draw(byte X, byte Y, byte N, byte NN, double_byte)
{
	assert(X + N < frame_buffer.size());
	assert(Y + BITS_PER_BYTE < frame_buffer[0].size());

	registers[0xf] = 0;
	for (size_t i = 0; i < N; ++i)
	{
		assert(index_register + i < memory.size());
		const auto bits = memory[index_register + i];
		for (size_t j = 0; j < BITS_PER_BYTE; ++j)
		{
			const auto bit = bits & (1 << (BITS_PER_BYTE - j - 1));
			if (bit)
			{
				frame_buffer[X + i][Y + j] = true;
				continue;
			}
			if (frame_buffer[X + i][Y + j])
			{
				registers[0xf] = 1;
			}
			frame_buffer[X + i][Y + j] = false;
		}
	}
}
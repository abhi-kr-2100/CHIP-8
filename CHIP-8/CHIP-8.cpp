#include <cassert>

#include "CHIP-8.hpp"

void CHIP_8::load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program)
{
	for (size_t i = 0; i < MAX_NUM_INSTRUCTIONS; ++i)
	{
		assert(i + PROGRAM_DATA_START_LOCATION < MEMORY_SIZE);
		memory[i + PROGRAM_DATA_START_LOCATION] = program[i];
	}
}

void CHIP_8::run()
{
	/* TODO */
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
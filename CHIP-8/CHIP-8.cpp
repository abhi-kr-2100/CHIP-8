#include <cassert>

#include "CHIP-8.hpp"
#include "executors-data.hpp"

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
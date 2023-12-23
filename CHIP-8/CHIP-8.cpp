#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include "CHIP-8.hpp"
#include "helpers.hpp"
#include "executor.hpp"

using std::pow;
using std::cin;
using std::exception;
using std::rand;

CHIP_8::CHIP_8()
	: pc{PROGRAM_DATA_START_LOCATION}, executor{new Executor(*this)}
{
	memory.fill(0);
	registers.fill(0);
	stack.fill(0);
	frame_buffer.fill({});

	load_fonts(FONT_DATA_START_LOCATION, FONT_DATA);
}

CHIP_8::~CHIP_8()
{
	delete executor;
}

void CHIP_8::load_program(const std::array<instruction_t,
		MAX_NUM_INSTRUCTIONS>& program)
{
	for (size_t ins_ctr = 0, mem_ctr = 0; ins_ctr < MAX_NUM_INSTRUCTIONS;
			++ins_ctr, mem_ctr += INSTRUCTION_SIZE)
	{
		for (size_t byte_ctr = 0; byte_ctr < INSTRUCTION_SIZE; ++byte_ctr)
		{
			const auto start_nibble = byte_ctr * NIBBLES_PER_BYTE;
			const auto mem_location = mem_ctr + byte_ctr + \
				PROGRAM_DATA_START_LOCATION;
			memory[mem_location] = get_nibbles_in_range(
				program[ins_ctr],
				start_nibble,
				start_nibble + NIBBLES_PER_BYTE - 1
			);
		}
	}
}

/**
 * Fetch and execute one instruction. Increment PC.
 *
 * Returns true if there are more instructions to execute, false otherwise.
 */
bool CHIP_8::run_one()
{
	const auto ins = get_current_instruction();
	pc += INSTRUCTION_SIZE;

	// 0x0000 is not a valid CHIP-8 instruction. However, empty memory cells
	// are 0. When a program has run to completion, PC points to empty memory
	// cells.
	if (ins.raw_instruction == 0)
	{
		return false;
	}

	executor->execute(ins);
	return true;
}

void
CHIP_8::load_fonts(double_byte start_loc, const decltype(FONT_DATA)& fonts)
{
	const auto nfont = fonts.size();
	const auto fontsz = fonts[0].size();

	for (size_t font_ctr = 0; font_ctr < nfont; ++font_ctr)
	{
		for (size_t byte_ctr = 0; byte_ctr < fontsz; ++byte_ctr)
		{
			const auto insert_loc = start_loc + font_ctr * fontsz + byte_ctr;
			assert(insert_loc < memory.size());
			memory[insert_loc] = fonts[font_ctr][byte_ctr];
		}
	}
}

void CHIP_8::clear_screen()
{
	for (size_t x = 0, width = frame_buffer.size(); x < width; ++x)
	{
		for (size_t y = 0, height = frame_buffer[0].size(); y < height; ++y)
		{
			frame_buffer[x][y] = false;
		}
	}
}

void CHIP_8::return_()
{
	if (stack_pointer == 0)
	{
		throw exception("No return address in stack.");
	}

	const auto return_addr = stack[stack_pointer--];
	jump(return_addr);
}

void CHIP_8::jump(double_byte NNN)
{
	assert(NNN < memory.size());
	pc = NNN;
}

void CHIP_8::set_register(byte X, byte NN)
{
	assert(X < registers.size());
	registers[X] = NN;
}

void CHIP_8::add(byte X, byte NN)
{
	assert(X < registers.size());
	registers[X] += NN;
}

void CHIP_8::set_index_register(double_byte NNN)
{
	index_register = NNN;
}

void CHIP_8::draw(byte X, byte Y, byte N)
{
	assert(X < registers.size());
	assert(Y < registers.size());

	const auto x = registers[X];
	const auto y = registers[Y];

	assert(x + BITS_PER_BYTE < FRAME_BUFFER_WIDTH);
	assert(y + N < FRAME_BUFFER_HEIGHT);

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
				frame_buffer[x + j][y + i] = true;
				continue;
			}
			if (frame_buffer[x + j][y + i])
			{
				registers[0xf] = 1;
			}
			frame_buffer[x + j][y + i] = false;
		}
	}
}

CHIP_8::Instruction CHIP_8::get_current_instruction() const
{
	// A CHIP-8 insturction is 2 bytes long; hence `pc + 1`.
	if (pc + 1 >= memory.size())
	{
		throw exception(
			"get_current_instruction: program counter points outside memory."
		);
	}

	const instruction_t ins = concatenate_bytes(memory.at(pc), memory.at(pc + 1));

	const byte category = get_nibbles_in_range(ins, 0, 0);

	const byte X = get_nibbles_in_range(ins, 1, 1);
	const byte Y = get_nibbles_in_range(ins, 2, 2);
	const byte N = get_nibbles_in_range(ins, 3, 3);
	const byte NN = get_nibbles_in_range(ins, 2, 3);
	const double_byte NNN = get_nibbles_in_range(ins, 1, 3);

	return Instruction
	{
		ins,
		category,
		{
			X, Y, N, NN, NNN
		},
	};
}

bool CHIP_8::get_pixel_at(size_t x, size_t y) const
{
	if (x >= frame_buffer.size() || y >= frame_buffer[0].size())
	{
		throw exception("get_pixel_at: coordinate out of screen.");
	}

	return frame_buffer.at(x).at(y);
}
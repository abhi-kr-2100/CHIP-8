#include <cstdlib>
#include <stdexcept>

#include "CHIP-8.hpp"
#include "helpers.hpp"
#include "executor.hpp"
#include "machine-specs.hpp"
#include "font-data.hpp"
#include "data-types.hpp"

using std::rand;
using std::out_of_range;

CHIP_8::CHIP_8()
	: pc{ PROGRAM_DATA_START_LOCATION }, executor{ new Executor(*this) }
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

void CHIP_8::load_program(const ROM& program)
{
	for (size_t i = 0, psz = program.size(); i < psz; ++i)
	{
		const auto ins = program[i];

		const auto mem_i = i * INSTRUCTION_SIZE;
		const auto mem_location = mem_i + PROGRAM_DATA_START_LOCATION;

		Helper::insert_instruction(*this, ins, mem_location);
	}
}

/**
 * Fetch and execute one instruction. Increment PC.
 *
 * Returns true if there are more instructions to execute, false otherwise.
 */
bool CHIP_8::run_one()
{
	if (is_blocked)
	{
		// Machine was blocked by the previous instruction. Keep going back to that
		// previous instruction until machine is unblocked.
		pc -= INSTRUCTION_SIZE;
	}

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
	const auto num_characters = fonts.size();
	const auto character_size = fonts[0].size();

	for (size_t char_i = 0; char_i < num_characters; ++char_i)
	{
		const auto char_start_loc = start_loc + char_i * character_size;
		for (size_t byte_i = 0; byte_i < character_size; ++byte_i)
		{
			const auto insert_loc = char_start_loc + byte_i;
			memory[insert_loc] = fonts[char_i][byte_i];
		}
	}
}

Instruction CHIP_8::get_current_instruction() const
{
	// A CHIP-8 insturction is 2 bytes long; hence `pc + 1`.
	if (pc + 1 >= memory.size())
	{
		throw out_of_range("get_current_instruction: pc points outside memory");
	}

	const instruction_t ins = concatenate_bytes(memory.at(pc), memory.at(pc + 1));
	return Helper::make_instruction_from_bytes(ins);
}

const Frame_buffer& CHIP_8::get_frame_buffer() const
{
	return frame_buffer;
}

void CHIP_8::decrement_timers(byte times)
{
	delay_timer = times >= delay_timer ? 0 : (delay_timer - times);
	sound_timer = times >= sound_timer ? 0 : (sound_timer - times);
}

void CHIP_8::Helper::insert_instruction(CHIP_8& machine, instruction_t ins, double_byte location)
{
	for (size_t byte_i = 0; byte_i < INSTRUCTION_SIZE; ++byte_i)
	{
		const auto start_nibble = byte_i * NIBBLES_PER_BYTE;
		machine.memory[location + byte_i] = get_nibbles_in_range(
			ins,
			start_nibble,
			start_nibble + NIBBLES_PER_BYTE - 1
		);
	}
}

Instruction CHIP_8::Helper::make_instruction_from_bytes(instruction_t bytes)
{
	const byte category = get_nibbles_in_range(bytes, 0, 0);

	const byte X = get_nibbles_in_range(bytes, 1, 1);
	const byte Y = get_nibbles_in_range(bytes, 2, 2);
	const byte N = get_nibbles_in_range(bytes, 3, 3);
	const byte NN = get_nibbles_in_range(bytes, 2, 3);
	const double_byte NNN = get_nibbles_in_range(bytes, 1, 3);

	return Instruction
	{
		bytes,
		category,
		{ X, Y, N, NN, NNN },
	};
}

double_byte CHIP_8::Helper::get_sprite_start_location(byte sprite_number)
{
	return FONT_DATA_START_LOCATION + FONT_CHAR_SIZE * sprite_number;
}

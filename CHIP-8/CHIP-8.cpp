#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include "CHIP-8.hpp"
#include "helpers.hpp"

using std::pow;
using std::cin;
using std::exception;
using std::rand;

CHIP_8::CHIP_8()
	: pc{PROGRAM_DATA_START_LOCATION}
{
	memory.fill(0);
	registers.fill(0);
	stack.fill(0);
	frame_buffer.fill({});

	load_fonts(FONT_DATA_START_LOCATION, FONT_DATA);

	executors[0x0] = &CHIP_8::ins_0;
	executors[0x1] = &CHIP_8::ins_1;
	executors[0x2] = &CHIP_8::ins_2;
	executors[0x3] = &CHIP_8::ins_3;
	executors[0x4] = &CHIP_8::ins_4;
	executors[0x5] = &CHIP_8::ins_5;
	executors[0x6] = &CHIP_8::ins_6;
	executors[0x7] = &CHIP_8::ins_7;
	executors[0x8] = &CHIP_8::ins_8;
	executors[0x9] = &CHIP_8::ins_9;
	executors[0xA] = &CHIP_8::ins_A;
	executors[0xB] = &CHIP_8::ins_B;
	executors[0xC] = &CHIP_8::ins_C;
	executors[0xD] = &CHIP_8::ins_D;
	executors[0xE] = &CHIP_8::ins_E;
	executors[0xF] = &CHIP_8::ins_F;
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

	(this->*executors[ins.category])(ins.payload);
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

void CHIP_8::ins_0(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.X == 0x0 && payload.Y == 0xE && payload.N == 0x0)
	{
		clear_screen();
	}
	else if (payload.X == 0x0 && payload.Y == 0xE && payload.N == 0xE)
	{
		return_();
	}
	else
	{
		throw exception("Machine call (0NNN) is not supported.");
	}
}

void CHIP_8::ins_1(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= memory.size())
	{
		throw exception("Jump address out of range.");
	}

	jump(payload.NNN);
}

void CHIP_8::ins_2(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= memory.size())
	{
		throw exception("Subroutine address out of range.");
	}

	if (stack_pointer >= stack.size())
	{
		throw exception("Subroutine call limit reached.");
	}

	stack[stack_pointer++] = pc;
	jump(payload.NNN);
}

void CHIP_8::ins_3(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (registers[payload.X] == payload.NN)
	{
		pc += 2;
	}
}

void CHIP_8::ins_4(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (registers[payload.X] != payload.NN)
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_5(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.N != 0)
	{
		throw exception("Unknown instruction. Category 0x5 instructions are of the form 0x5XY0.");
	}

	if (registers[payload.X] == registers[payload.Y])
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_6(const CHIP_8::Instruction::Instruction_payload& payload)
{
	set_register(payload.X, payload.NN);
}

void CHIP_8::ins_7(const CHIP_8::Instruction::Instruction_payload& payload)
{
	add(payload.X, payload.NN);
}

void CHIP_8::ins_8(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.N)
	{
	case 0x0:
		set_register(payload.X, registers[payload.Y]);
		break;
	case 0x1:
		set_register(payload.X, registers[payload.X] | registers[payload.Y]);
		break;
	case 0x2:
		set_register(payload.X, registers[payload.X] & registers[payload.Y]);
		break;
	case 0x3:
		set_register(payload.X, registers[payload.X] ^ registers[payload.Y]);
		break;
	case 0x4:
	{
		const auto sum = registers[payload.X] + registers[payload.Y];
		set_register(payload.X, sum);
		set_register(0xF, sum > 0xFF);
		break;
	}
	case 0x5:
	{
		const auto diff = registers[payload.X] - registers[payload.Y];
		set_register(payload.X, diff);
		set_register(0xF, diff >= 0);
		break;
	}
	case 0x6:
	{
		const auto lsb = registers[payload.X] & 0x1;
		set_register(0xF, lsb);
		set_register(payload.X, registers[payload.X] >> 1);
		break;
	}
	case 0x7:
	{
		const auto diff = registers[payload.Y] - registers[payload.X];
		set_register(payload.X, diff);
		set_register(0xF, diff >= 0);
		break;
	}
	case 0xE:
	{
		const auto msb = registers[payload.X] & (1 << (BITS_PER_BYTE - 1));
		set_register(0xF, msb);
		set_register(payload.X, registers[payload.X] << 1);
		break;
	}
	default:
		throw exception("Invalid category 8 instruction format.");
	}
}

void CHIP_8::ins_9(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.N != 0)
	{
		throw exception("Invalid category 9 instruction format.");
	}

	if (registers[payload.X] != registers[payload.Y])
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_A(const CHIP_8::Instruction::Instruction_payload& payload)
{
	set_index_register(payload.NNN);
}

void CHIP_8::ins_B(const CHIP_8::Instruction::Instruction_payload& payload)
{
	jump(registers[0] + payload.NNN);
}

void CHIP_8::ins_C(const CHIP_8::Instruction::Instruction_payload& payload)
{
	const auto random = rand() % (int)pow(2, BITS_PER_BYTE);
	set_register(payload.X, random & payload.NN);
}

void CHIP_8::ins_D(const CHIP_8::Instruction::Instruction_payload& payload)
{
	draw(payload.X, payload.Y, payload.N);
}

void CHIP_8::ins_E(const CHIP_8::Instruction::Instruction_payload& payload)
{
	throw exception("Not implemented.");
}

void CHIP_8::ins_F(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.NN)
	{
	case 0x07:
		set_register(payload.X, delay_timer);
		break;
	case 0x0A:
	{
		char ch;
		cin >> ch;
		// TODO: decode ch to correct key
		set_register(payload.X, ch);
		break;
	}
	case 0x15:
	{
		delay_timer = registers[payload.X];
		break;
	}
	case 0x18:
	{
		sound_timer = registers[payload.X];
		break;
	}
	case 0x1E:
		set_index_register(index_register + registers[payload.X]);
		break;
	case 0x29:
	{
		// TODO: Implement this
		break;
	}
	case 0x33:
	{
		// TODO: Implement this
		break;
	}
	case 0x55:
	{
		// TODO: Implement this
		break;
	}
	case 0x65:
	{
		// TODO: Implement this
		break;
	}
	default:
		throw exception("Invalid category F instruction format.");
		break;
	}
}

void CHIP_8::clear_screen()
{
	for (size_t x = 0, width = FRAME_BUFFER_WIDTH; x < width; ++x)
	{
		for (size_t y = 0, height = FRAME_BUFFER_HEIGHT; y < height; ++y)
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
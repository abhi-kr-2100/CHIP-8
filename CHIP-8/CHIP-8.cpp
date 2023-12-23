#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include <ftxui/screen/screen.hpp>

#include "CHIP-8.hpp"
#include "helpers.hpp"

using std::pow;
using std::cin;
using std::exception;
using std::rand;
using ftxui::Screen;
using ftxui::Dimension::Fixed;

CHIP_8::CHIP_8()
	: pc{PROGRAM_DATA_START_LOCATION}, frame_buffer{Screen::Create(Fixed(FRAME_BUFFER_WIDTH), Fixed(FRAME_BUFFER_HEIGHT))}
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
	for (size_t x = 0; x < FRAME_BUFFER_WIDTH; ++x)
	{
		for (size_t y = 0; y < FRAME_BUFFER_HEIGHT; ++y)
		{
			set_frame_buffer_pixel(x, y, false);
		}
	}

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
	while (true)
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
	}
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

void CHIP_8::ins_0(byte X, byte Y, byte N, byte, double_byte)
{
	if (X == 0x0 && Y == 0xE && N == 0x0)
	{
		clear_screen();
	}
	else if (X == 0x0 && Y == 0xE && N == 0xE)
	{
		return_();
	}
	else
	{
		throw exception("Machine call (0NNN) is not supported.");
	}
}

void CHIP_8::ins_1(byte, byte, byte, byte, double_byte NNN)
{
	if (NNN >= memory.size())
	{
		throw exception("Jump address out of range.");
	}

	jump(NNN);
}

void CHIP_8::ins_2(byte, byte, byte, byte, double_byte NNN)
{
	if (NNN >= memory.size())
	{
		throw exception("Subroutine address out of range.");
	}

	if (stack_pointer >= stack.size())
	{
		throw exception("Subroutine call limit reached.");
	}

	stack[stack_pointer++] = pc;
	jump(NNN);
}

void CHIP_8::ins_3(byte X, byte, byte, byte NN, double_byte)
{
	if (registers[X] == NN)
	{
		pc += 2;
	}
}

void CHIP_8::ins_4(byte X, byte, byte, byte NN, double_byte)
{
	if (registers[X] != NN)
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_5(byte X, byte Y, byte N, byte, double_byte)
{
	if (N != 0)
	{
		throw exception("Unknown instruction. Category 0x5 instructions are of the form 0x5XY0.");
	}

	if (registers[X] == registers[Y])
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_6(byte X, byte, byte, byte NN, double_byte)
{
	set_register(X, NN);
}

void CHIP_8::ins_7(byte X, byte, byte, byte NN, double_byte)
{
	add(X, NN);
}

void CHIP_8::ins_8(byte X, byte Y, byte N, byte, double_byte)
{
	switch (N)
	{
	case 0x0:
		set_register(X, registers[Y]);
		break;
	case 0x1:
		set_register(X, registers[X] | registers[Y]);
		break;
	case 0x2:
		set_register(X, registers[X] & registers[Y]);
		break;
	case 0x3:
		set_register(X, registers[X] ^ registers[Y]);
		break;
	case 0x4:
	{
		const auto sum = registers[X] + registers[Y];
		set_register(X, sum);
		set_register(0xF, sum > 0xFF);
		break;
	}
	case 0x5:
	{
		const auto diff = registers[X] - registers[Y];
		set_register(X, diff);
		set_register(0xF, diff >= 0);
		break;
	}
	case 0x6:
	{
		const auto lsb = registers[X] & 0x1;
		set_register(0xF, lsb);
		set_register(X, registers[X] >> 1);
		break;
	}
	case 0x7:
	{
		const auto diff = registers[Y] - registers[X];
		set_register(X, diff);
		set_register(0xF, diff >= 0);
		break;
	}
	case 0xE:
	{
		const auto msb = registers[X] & (1 << (BITS_PER_BYTE - 1));
		set_register(0xF, msb);
		set_register(X, registers[X] << 1);
		break;
	}
	default:
		throw exception("Invalid category 8 instruction format.");
	}
}

void CHIP_8::ins_9(byte X, byte Y, byte N, byte, double_byte)
{
	if (N != 0)
	{
		throw exception("Invalid category 9 instruction format.");
	}

	if (registers[X] != registers[Y])
	{
		pc += INSTRUCTION_SIZE;
	}
}

void CHIP_8::ins_A(byte, byte, byte, byte, double_byte NNN)
{
	set_index_register(NNN);
}

void CHIP_8::ins_B(byte, byte, byte, byte, double_byte NNN)
{
	jump(registers[0] + NNN);
}

void CHIP_8::ins_C(byte X, byte, byte, byte NN, double_byte)
{
	const auto random = rand() % (int)pow(2, BITS_PER_BYTE);
	set_register(X, random & NN);
}

void CHIP_8::ins_D(byte X, byte Y, byte N, byte, double_byte)
{
	draw(X, Y, N);
}

void CHIP_8::ins_E(byte X, byte, byte, byte NN, double_byte)
{
	throw exception("Not implemented.");
}

void CHIP_8::ins_F(byte X, byte, byte, byte NN, double_byte)
{
	switch (NN)
	{
	case 0x07:
		set_register(X, delay_timer);
		break;
	case 0x0A:
	{
		char ch;
		cin >> ch;
		// TODO: decode ch to correct key
		set_register(X, ch);
		break;
	}
	case 0x15:
	{
		delay_timer = registers[X];
		break;
	}
	case 0x18:
	{
		sound_timer = registers[X];
		break;
	}
	case 0x1E:
		set_index_register(index_register + registers[X]);
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
			set_frame_buffer_pixel(x, y, false);
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
				set_frame_buffer_pixel(x + j, y + i, true);
				continue;
			}
			if (get_frame_buffer_pixel(x + j, y + i))
			{
				registers[0xf] = 1;
			}
			set_frame_buffer_pixel(x + j, y + i, false);
		}
	}
}

bool CHIP_8::get_frame_buffer_pixel(size_t x, size_t y)
{
	assert(x < FRAME_BUFFER_WIDTH);
	assert(y < FRAME_BUFFER_HEIGHT);

	return frame_buffer.at(x, y) == "*";
}

void CHIP_8::set_frame_buffer_pixel(size_t x, size_t y, bool val)
{
	assert(x < FRAME_BUFFER_WIDTH);
	assert(y < FRAME_BUFFER_HEIGHT);

	frame_buffer.at(x, y) = val ? "*" : " ";
	frame_buffer.Print();
}
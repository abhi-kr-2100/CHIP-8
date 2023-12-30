#include <exception>
#include <vector>
#include <cstdlib>
#include <cmath>

#include "executor.hpp"
#include "CHIP-8.hpp"
#include "helpers.hpp"
#include "keyboard.hpp"
#include "machine-specs.hpp"
#include "data-types.hpp"

using std::exception;
using std::vector;
using std::pow;
using std::rand;

Executor::Executor(CHIP_8& machine)
	: machine{ machine }, executors{
		&Executor::category_0, &Executor::jump, &Executor::subroutine_call,
		&Executor::skip_if_vx_eq_nn, &Executor::skip_if_vx_neq_nn,
		&Executor::skip_if_vx_eq_vy, &Executor::set_register,
		&Executor::inc_reg_by_const, &Executor::operate_and_assign,
		&Executor::skip_if_vx_neq_vy, &Executor::set_index_register,
		&Executor::jump_with_offset, &Executor::set_random, &Executor::draw,
		&Executor::skip_cond_key, &Executor::category_F
	}
{
}

void Executor::execute(const Instruction& ins)
{
	(this->*executors[ins.category])(ins.payload);
}

void Executor::category_0(const Instruction::Instruction_payload& payload)
{
	if (payload.X == 0x0 && payload.Y == 0xE && payload.N == 0x0)
	{
		Helper::clear_screen(machine);
	}
	else if (payload.X == 0x0 && payload.Y == 0xE && payload.N == 0xE)
	{
		Helper::return_(machine);
	}
	else
	{
		throw exception("Machine call (0NNN) is not supported.");
	}
}

void Executor::jump(const Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= machine.memory.size())
	{
		throw exception("Jump address out of range.");
	}

	machine.pc = payload.NNN;
}

void Executor::subroutine_call(const Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= machine.memory.size())
	{
		throw exception("Subroutine address out of range.");
	}

	if (machine.stack_pointer >= machine.stack.size())
	{
		throw exception("Subroutine call limit reached.");
	}

	machine.stack[machine.stack_pointer++] = machine.pc;
	machine.pc = payload.NNN;
}

void Executor::skip_if_vx_eq_nn(const Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] == payload.NN)
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::skip_if_vx_neq_nn(const Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] != payload.NN)
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::skip_if_vx_eq_vy(const Instruction::Instruction_payload& payload)
{
	if (payload.N != 0)
	{
		throw exception("Unknown instruction. Category 0x5 instructions are of the form 0x5XY0.");
	}

	if (machine.registers[payload.X] == machine.registers[payload.Y])
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::set_register(const Instruction::Instruction_payload& payload)
{
	machine.registers[payload.X] = payload.NN;
}

void Executor::inc_reg_by_const(const Instruction::Instruction_payload& payload)
{
	machine.registers[payload.X] += payload.NN;
}

void Executor::operate_and_assign(const Instruction::Instruction_payload& payload)
{
	switch (payload.N)
	{
	case 0x0:
		machine.registers[payload.X] = machine.registers[payload.Y];
		break;
	case 0x1:
		machine.registers[payload.X] |= machine.registers[payload.Y];
		machine.registers[0xF] = 0;
		break;
	case 0x2:
		machine.registers[payload.X] &= machine.registers[payload.Y];
		machine.registers[0xF] = 0;
		break;
	case 0x3:
		machine.registers[payload.X] ^= machine.registers[payload.Y];
		machine.registers[0xF] = 0;
		break;
	case 0x4:
	{
		const auto sum = machine.registers[payload.X] + machine.registers[payload.Y];
		machine.registers[payload.X] = sum;
		machine.registers[0xF] = sum > 0xFF;
		break;
	}
	case 0x5:
	{
		const auto diff = machine.registers[payload.X] - machine.registers[payload.Y];
		machine.registers[payload.X] = diff;
		machine.registers[0xF] = diff >= 0;
		break;
	}
	case 0x6:
	{
		machine.registers[payload.X] = machine.registers[payload.Y];
		const auto lsb = get_least_significant_bit(machine.registers[payload.X]);
		machine.registers[payload.X] >>= 1;
		machine.registers[0xF] = lsb;
		break;
	}
	case 0x7:
	{
		const auto diff = machine.registers[payload.Y] - machine.registers[payload.X];
		machine.registers[payload.X] = diff;
		machine.registers[0xF] = diff >= 0;
		break;
	}
	case 0xE:
	{
		machine.registers[payload.X] = machine.registers[payload.Y];
		const auto msb = get_most_significant_bit(machine.registers[payload.X]);
		machine.registers[payload.X] <<= 1;
		machine.registers[0xF] = msb;
		break;
	}
	default:
		throw exception("Invalid category 8 instruction format.");
	}
}

void Executor::skip_if_vx_neq_vy(const Instruction::Instruction_payload& payload)
{
	if (payload.N != 0)
	{
		throw exception("Invalid category 9 instruction format.");
	}

	if (machine.registers[payload.X] != machine.registers[payload.Y])
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::set_index_register(const Instruction::Instruction_payload& payload)
{
	machine.index_register = payload.NNN;
}

void Executor::jump_with_offset(const Instruction::Instruction_payload& payload)
{
	machine.pc = machine.registers[0] + payload.NNN;
}

void Executor::set_random(const Instruction::Instruction_payload& payload)
{
	const auto random = rand() % (int)pow(2, BITS_PER_BYTE);
	machine.registers[payload.X] = random & payload.NN;
}

void Executor::draw(const Instruction::Instruction_payload& payload)
{
	const auto x = machine.registers[payload.X];
	const auto y = machine.registers[payload.Y];

	const auto fb_width = machine.frame_buffer.size();
	const auto fb_height = machine.frame_buffer[0].size();

	byte vf_flag_val = 0;
	for (size_t i = 0; i < payload.N; ++i)
	{
		const auto bits = machine.memory[machine.index_register + i];
		for (size_t j = 0; j < BITS_PER_BYTE; ++j)
		{
			const auto bit = bits & (1 << (BITS_PER_BYTE - j - 1));
			if (!bit)
			{
				machine.frame_buffer[(x + j) % fb_width][(y + i) % fb_height] ^= 0;
				continue;
			}
			if (machine.frame_buffer[(x + j) % fb_width][(y + i) % fb_height])
			{
				vf_flag_val = 1;
			}
			machine.frame_buffer[(x + j) % fb_width][(y + i) % fb_height] ^= 1;
		}
	}

	machine.registers[0xF] = vf_flag_val;
}

void Executor::skip_cond_key(const Instruction::Instruction_payload& payload)
{
	switch (payload.NN)
	{
	case 0x9E:
	{
		const auto key = static_cast<Key>(machine.registers[payload.X]);
		if (machine.keyboard.is_key_pressed(key))
		{
			machine.pc += INSTRUCTION_SIZE;
		}
		break;
	}
	case 0xA1:
	{
		const auto key = static_cast<Key>(machine.registers[payload.X]);
		if (!machine.keyboard.is_key_pressed(key))
		{
			machine.pc += INSTRUCTION_SIZE;
		}
		break;
	}
	default:
		throw exception("skip_cond_key: badly formatted category E instruction.");
	}
}

void Executor::category_F(const Instruction::Instruction_payload& payload)
{
	switch (payload.NN)
	{
	case 0x07:
		machine.registers[payload.X] = machine.delay_timer;
		break;
	case 0x0A:
	{
		machine.is_blocked = true;
		for (Key k = Key::K0; k <= Key::KF; k = static_cast<Key>((int)k + 1))
		{
			if (machine.keyboard.is_key_pressed(k))
			{
				machine.registers[payload.X] = (int)k;
				machine.is_blocked = false;
				break;
			}
		}
		break;
	}
	case 0x15:
		machine.delay_timer = machine.registers[payload.X];
		break;
	case 0x18:
		machine.sound_timer = machine.registers[payload.X];
		break;
	case 0x1E:
		machine.index_register += machine.registers[payload.X];
		break;
	case 0x29:
	{
		const auto num = machine.registers[payload.X];
		machine.index_register = CHIP_8::Helper::get_sprite_start_location(num);
		break;
	}
	case 0x33:
	{
		const auto digits = get_digits(machine.registers[payload.X]);
		for (size_t i = 0, sz = digits.size(); i < sz; ++i)
		{
			machine.memory[machine.index_register + i] = digits[i];
		}
		break;
	}
	case 0x55:
	{
		for (size_t i = 0; i <= payload.X; ++i)
		{
			machine.memory[machine.index_register++] = machine.registers[i];
		}
		break;
	}
	case 0x65:
	{
		for (size_t i = 0; i <= payload.X; ++i)
		{
			machine.registers[i] = machine.memory[machine.index_register++];
		}
		break;
	}
	default:
		throw exception("Invalid category F instruction format.");
		break;
	}
}

void Executor::Helper::clear_screen(CHIP_8& machine)
{
	machine.frame_buffer.fill({});
}

void Executor::Helper::return_(CHIP_8& machine)
{
	if (machine.stack_pointer == 0)
	{
		throw exception("return_: no address to return to.");
	}

	const double_byte return_addr = machine.stack[--machine.stack_pointer];
	machine.pc = return_addr;
}
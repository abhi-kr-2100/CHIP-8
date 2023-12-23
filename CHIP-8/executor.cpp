#include <stdexcept>

#include "executor.hpp"
#include "CHIP-8.hpp"

using std::exception;

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

void Executor::execute(const CHIP_8::Instruction& ins)
{
	(this->*executors[ins.category])(ins.payload);
}

void Executor::category_0(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::jump(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= machine.memory.size())
	{
		throw exception("Jump address out of range.");
	}

	machine.pc = payload.NNN;
}

void Executor::subroutine_call(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::skip_if_vx_eq_nn(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] == payload.NN)
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::skip_if_vx_neq_nn(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] != payload.NN)
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::skip_if_vx_eq_vy(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::set_register(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.registers[payload.X] = payload.NN;
}

void Executor::inc_reg_by_const(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.registers[payload.X] += payload.NN;
}

void Executor::operate_and_assign(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.N)
	{
	case 0x0:
		machine.registers[payload.X] = machine.registers[payload.Y];
		break;
	case 0x1:
		machine.registers[payload.X] |= machine.registers[payload.Y];
		break;
	case 0x2:
		machine.registers[payload.X] &= machine.registers[payload.Y];
		break;
	case 0x3:
		machine.registers[payload.X] ^= machine.registers[payload.Y];
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
		const auto lsb = machine.registers[payload.X] & 0x1;
		machine.registers[0xF] = lsb;
		machine.registers[payload.X] >>= 1;
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
		const auto msb = machine.registers[payload.X] & (1 << (BITS_PER_BYTE - 1));
		machine.registers[0xF] = msb;
		machine.registers[payload.X] <<= 1;
		break;
	}
	default:
		throw exception("Invalid category 8 instruction format.");
	}
}

void Executor::skip_if_vx_neq_vy(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::set_index_register(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.index_register = payload.NNN;
}

void Executor::jump_with_offset(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.pc = machine.registers[0] + payload.NNN;
}

void Executor::set_random(const CHIP_8::Instruction::Instruction_payload& payload)
{
	const auto random = rand() % (int)pow(2, BITS_PER_BYTE);
	machine.registers[payload.X] = random & payload.NN;
}

void Executor::draw(const CHIP_8::Instruction::Instruction_payload& payload)
{
	const auto x = machine.registers[payload.X];
	const auto y = machine.registers[payload.Y];

	machine.registers[0xf] = 0;
	for (size_t i = 0; i < payload.N; ++i)
	{
		const auto bits = machine.memory[machine.index_register + i];
		for (size_t j = 0; j < BITS_PER_BYTE; ++j)
		{
			const auto bit = bits & (1 << (BITS_PER_BYTE - j - 1));
			if (bit)
			{
				machine.frame_buffer[x + j][y + i] = true;
				continue;
			}
			if (machine.frame_buffer[x + j][y + i])
			{
				machine.registers[0xf] = 1;
			}
			machine.frame_buffer[x + j][y + i] = false;
		}
	}
}

void Executor::skip_cond_key(const CHIP_8::Instruction::Instruction_payload& payload)
{
	throw exception("Not implemented.");
}

void Executor::category_F(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.NN)
	{
	case 0x07:
		machine.registers[payload.X] = machine.delay_timer;
		break;
	case 0x0A:
		// TODO: Implement this
		break;
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
		// TODO: Implement this
		break;
	case 0x33:
		// TODO: Implement this
		break;
	case 0x55:
		// TODO: Implement this
		break;
	case 0x65:
		// TODO: Implement this
		break;
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

	const auto return_addr = machine.stack[machine.stack_pointer--];
	machine.pc = return_addr;
}
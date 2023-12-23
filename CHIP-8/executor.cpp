#include <stdexcept>

#include "executor.hpp"
#include "CHIP-8.hpp"

using std::exception;

Executor::Executor(CHIP_8& machine)
	: machine{ machine }, executors{
		&Executor::category_0, &Executor::category_1, &Executor::category_2,
		&Executor::category_3, &Executor::category_4, &Executor::category_5,
		&Executor::category_6, &Executor::category_7, &Executor::category_8,
		&Executor::category_9, &Executor::category_A, &Executor::category_B,
		&Executor::category_C, &Executor::category_D, &Executor::category_E,
		&Executor::category_F
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
		machine.clear_screen();
	}
	else if (payload.X == 0x0 && payload.Y == 0xE && payload.N == 0xE)
	{
		machine.return_();
	}
	else
	{
		throw exception("Machine call (0NNN) is not supported.");
	}
}

void Executor::category_1(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (payload.NNN >= machine.memory.size())
	{
		throw exception("Jump address out of range.");
	}

	machine.jump(payload.NNN);
}

void Executor::category_2(const CHIP_8::Instruction::Instruction_payload& payload)
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
	machine.jump(payload.NNN);
}

void Executor::category_3(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] == payload.NN)
	{
		machine.pc += 2;
	}
}

void Executor::category_4(const CHIP_8::Instruction::Instruction_payload& payload)
{
	if (machine.registers[payload.X] != payload.NN)
	{
		machine.pc += INSTRUCTION_SIZE;
	}
}

void Executor::category_5(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::category_6(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.set_register(payload.X, payload.NN);
}

void Executor::category_7(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.add(payload.X, payload.NN);
}

void Executor::category_8(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.N)
	{
	case 0x0:
		machine.set_register(payload.X, machine.registers[payload.Y]);
		break;
	case 0x1:
		machine.set_register(payload.X, machine.registers[payload.X] | machine.registers[payload.Y]);
		break;
	case 0x2:
		machine.set_register(payload.X, machine.registers[payload.X] & machine.registers[payload.Y]);
		break;
	case 0x3:
		machine.set_register(payload.X, machine.registers[payload.X] ^ machine.registers[payload.Y]);
		break;
	case 0x4:
	{
		const auto sum = machine.registers[payload.X] + machine.registers[payload.Y];
		machine.set_register(payload.X, sum);
		machine.set_register(0xF, sum > 0xFF);
		break;
	}
	case 0x5:
	{
		const auto diff = machine.registers[payload.X] - machine.registers[payload.Y];
		machine.set_register(payload.X, diff);
		machine.set_register(0xF, diff >= 0);
		break;
	}
	case 0x6:
	{
		const auto lsb = machine.registers[payload.X] & 0x1;
		machine.set_register(0xF, lsb);
		machine.set_register(payload.X, machine.registers[payload.X] >> 1);
		break;
	}
	case 0x7:
	{
		const auto diff = machine.registers[payload.Y] - machine.registers[payload.X];
		machine.set_register(payload.X, diff);
		machine.set_register(0xF, diff >= 0);
		break;
	}
	case 0xE:
	{
		const auto msb = machine.registers[payload.X] & (1 << (BITS_PER_BYTE - 1));
		machine.set_register(0xF, msb);
		machine.set_register(payload.X, machine.registers[payload.X] << 1);
		break;
	}
	default:
		throw exception("Invalid category 8 instruction format.");
	}
}

void Executor::category_9(const CHIP_8::Instruction::Instruction_payload& payload)
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

void Executor::category_A(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.set_index_register(payload.NNN);
}

void Executor::category_B(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.jump(machine.registers[0] + payload.NNN);
}

void Executor::category_C(const CHIP_8::Instruction::Instruction_payload& payload)
{
	const auto random = rand() % (int)pow(2, BITS_PER_BYTE);
	machine.set_register(payload.X, random & payload.NN);
}

void Executor::category_D(const CHIP_8::Instruction::Instruction_payload& payload)
{
	machine.draw(payload.X, payload.Y, payload.N);
}

void Executor::category_E(const CHIP_8::Instruction::Instruction_payload& payload)
{
	throw exception("Not implemented.");
}

void Executor::category_F(const CHIP_8::Instruction::Instruction_payload& payload)
{
	switch (payload.NN)
	{
	case 0x07:
		machine.set_register(payload.X, machine.delay_timer);
		break;
	case 0x0A:
	{
		// TODO: Implement this
		break;
	}
	case 0x15:
	{
		machine.delay_timer = machine.registers[payload.X];
		break;
	}
	case 0x18:
	{
		machine.sound_timer = machine.registers[payload.X];
		break;
	}
	case 0x1E:
		machine.set_index_register(machine.index_register + machine.registers[payload.X]);
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
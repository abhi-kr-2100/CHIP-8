#include <array>

#include "CHIP-8.hpp"
#include "debugger.hpp"
#include "data-types.hpp"
#include "machine-specs.hpp"

Debugger::Debugger(CHIP_8& machine)
	: machine{ machine }
{
}

bool Debugger::run_one()
{
	const auto& executed_instruction = machine.get_current_instruction();
	const auto can_run_more = run_one_without_callback();

	for (const auto& f : callbacks)
	{
		f(Execution_event::RUN_ONE, executed_instruction);
	}

	return can_run_more;
}

bool Debugger::go_back_one()
{
	const auto can_go_back_more = go_back_one_without_callback();
	const auto& undone_instruction = machine.get_current_instruction();

	for (const auto& f : callbacks)
	{
		f(Execution_event::GO_BACK_ONE, undone_instruction);
	}

	return can_go_back_more;
}

bool Debugger::run_one_without_callback()
{
	const auto current_state = Machine_state{
		machine.memory,
		machine.registers,
		machine.stack,
		machine.frame_buffer,
		machine.pc,
		machine.index_register,
		machine.stack_pointer,
		machine.delay_timer,
		machine.sound_timer,
		machine.is_blocked,
	};
	states.push(current_state);

	return machine.run_one();
}

bool Debugger::go_back_one_without_callback()
{
	const auto& most_recent_state = states.top();
	states.pop();

	machine.load_state(most_recent_state);

	return !states.empty();
}

void Debugger::on_exec(const std::function<void(Execution_event, const Instruction&)>& f)
{
	callbacks.push_back(f);
}

const std::array<byte, MEMORY_SIZE>& Debugger::get_memory() const
{
	return machine.memory;
}

const std::array<byte, NUM_REGISTERS>& Debugger::get_registers() const
{
	return machine.registers;
}

byte Debugger::get_register(size_t i) const
{
	return machine.registers[i];
}

double_byte Debugger::get_pc() const
{
	return machine.pc;
}

double_byte Debugger::get_index_register() const
{
	return machine.index_register;
}

void Debugger::set_memory_byte(size_t location, byte value)
{
	machine.memory[location] = value;
}

void Debugger::set_register(size_t i, byte value)
{
	machine.registers[i] = value;
}

void Debugger::set_pc(double_byte value)
{
	machine.pc = value;
}

void Debugger::set_index_register(double_byte value)
{
	machine.index_register = value;
}

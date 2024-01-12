#include "CHIP-8.hpp"
#include "debugger.hpp"
#include "data-types.hpp"

Debugger::Debugger(CHIP_8& machine)
	: machine{ machine }
{
}

bool Debugger::run_one()
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

bool Debugger::go_back_one()
{
	const auto& most_recent_state = states.top();
	states.pop();

	machine.load_state(most_recent_state);

	return !states.empty();
}

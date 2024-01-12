#pragma once

#include <stack>

#include "CHIP-8.hpp"
#include "data-types.hpp"

class Debugger
{
public:
	Debugger(CHIP_8& machine);

	bool run_one();
	bool go_back_one();
private:
	CHIP_8 machine;
	std::stack<Machine_state> states;
};
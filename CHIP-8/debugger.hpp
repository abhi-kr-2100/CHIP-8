#pragma once

#include <stack>
#include <array>

#include "CHIP-8.hpp"
#include "machine-specs.hpp"
#include "data-types.hpp"

class Debugger
{
public:
	Debugger(CHIP_8& machine);

	bool run_one();
	bool go_back_one();

	const std::array<byte, MEMORY_SIZE>& get_memory() const;
	byte get_register(size_t i) const;
	double_byte get_pc() const;
	double_byte get_index_register() const;

	void set_memory_byte(size_t location, byte value);
	void set_register(size_t i, byte value);
	void set_pc(double_byte value);
	void set_index_register(double_byte value);
private:
	CHIP_8& machine;
	std::stack<Machine_state> states;
};
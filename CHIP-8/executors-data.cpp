#include <iostream>

#include "executors-data.hpp"

using std::cout;

void clear_screen(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	for (size_t i = 0, width = machine.get_framebuffer_width(); i < width; ++i)
	{
		for (size_t j = 0, height = machine.get_framebuffer_height(); j < height; ++j)
		{
			machine.unset_framebuffer_pixel(i, j);
		}
	}
}

void jump(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	machine.set_pc(NNN);
}

void set_register(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	machine.set_register(X, NN);
}

void add(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	const auto old_value = machine.get_register(X);
	const auto new_value = old_value + NN;
	machine.set_register(X, new_value);
}

void set_index_register(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	machine.set_index_register(NNN);
}

void draw(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "draw called.\n";
}

#include <iostream>

#include "executors-data.hpp"

using std::cout;

void clear_screen(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	for (size_t i = 0; i < FRAME_BUFFER_WIDTH; ++i)
	{
		for (size_t j = 0; j < FRAME_BUFFER_HEIGHT; ++j)
		{
			machine.unset_framebuffer_pixel(i, j);
		}
	}
}

void jump(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "jump called.\n";
}

void set_register(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "set_register called.\n";
}

void add(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "add called.\n";
}

void set_index_register(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "set_index_register called.\n";
}

void draw(byte X, byte Y, byte N, byte NN, double_byte NNN, CHIP_8& machine)
{
	cout << "draw called.\n";
}

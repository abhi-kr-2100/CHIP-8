#include <iostream>

#include "executors-data.hpp"

using std::cout;

void clear_screen(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "clear_screen called.\n";
}

void jump(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "jump called.\n";
}

void set_register(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "set_register called.\n";
}

void add(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "add called.\n";
}

void set_index_register(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "set_index_register called.\n";
}

void draw(nibble X, nibble Y, nibble N, byte NN, double_byte NNN)
{
	cout << "draw called.\n";
}

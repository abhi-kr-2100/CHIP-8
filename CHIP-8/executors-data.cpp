#include <iostream>

#include "executors-data.hpp"

using std::cout;

void clear_screen(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "clear_screen called.\n";
}

void jump(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "jump called.\n";
}

void set_register(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "set_register called.\n";
}

void add(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "add called.\n";
}

void set_index_register(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "set_index_register called.\n";
}

void draw(byte X, byte Y, byte N, byte NN, double_byte NNN)
{
	cout << "draw called.\n";
}

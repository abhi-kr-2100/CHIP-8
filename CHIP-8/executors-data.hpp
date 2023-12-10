#pragma once

#include "CHIP-8.hpp"

void clear_screen(byte X, byte Y, byte N, byte NN, double_byte NNN);
void jump(byte X, byte Y, byte N, byte NN, double_byte NNN);
void set_register(byte X, byte Y, byte N, byte NN, double_byte NNN);
void add(byte X, byte Y, byte N, byte NN, double_byte NNN);
void set_index_register(byte X, byte Y, byte N, byte NN, double_byte NNN);
void draw(byte X, byte Y, byte N, byte NN, double_byte NNN);
#pragma once

#include "CHIP-8.hpp"

void clear_screen(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
void jump(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
void set_register(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
void add(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
void set_index_register(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
void draw(nibble X, nibble Y, nibble N, byte NN, double_byte NNN);
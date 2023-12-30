#pragma once

#include <cstdint>
#include <array>

#include "machine-specs.hpp"

using byte = std::uint8_t;
using double_byte = std::uint16_t;
using instruction_t = double_byte;

using Frame_buffer = std::array<std::array<byte, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>;
using ROM = std::array<instruction_t, MAX_NUM_INSTRUCTIONS>;

/**
 * A CHIP-8 instruction is 2 bytes (or 4 nibbles) long, and can be
 * represented using 4 hexadecimal digits (one for each nibble).
 *
 * The leftmost digit is the category which determines how the
 * remaining 3 digits are interpreted.
 *
 * The various interpretations of the other nibbles are as follows:
 * - X: the 2nd nibble
 * - Y: the 3rd nibble
 * - N: the 4th nibble
 * - NN: the 2nd byte
 * - NNN: the 2nd, 3rd, and the 4th nibbles
 */
struct Instruction
{
	struct Instruction_payload
	{
		byte X;
		byte Y;
		byte N;
		byte NN;
		double_byte NNN;
	};

	instruction_t raw_instruction;
	byte category;
	Instruction_payload payload;
};
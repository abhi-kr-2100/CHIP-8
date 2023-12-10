#pragma once

#include "data-types.hpp"
#include "machine-specs.hpp"

double_byte concatenate_bytes(byte b1, byte b2)
{
	return b1 << BITS_PER_BYTE | b2;
}
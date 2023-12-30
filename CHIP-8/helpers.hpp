#pragma once

#include <vector>

#include "data-types.hpp"

double_byte concatenate_bytes(byte b1, byte b2);

/**
 * A double_byte is made up of 4 nibbles, which we number from 0 till 3.
 * This function returns a double_byte derived from the given double_byte where
 * the nibbles not in the specified range are set to 0.
 *
 * Example: Let's say the given double_byte contains the value 0xABCD. We can
 * request nibbles of in the range 1 to 2, i.e., 0xBC.
 * get_nibbles_in_range(0xABCD, 1, 2) will return 0x00BC.
 */
double_byte get_nibbles_in_range(double_byte b, int first, int last);

std::vector<int> get_digits(size_t num);

byte get_most_significant_bit(byte num);
byte get_least_significant_bit(byte num);
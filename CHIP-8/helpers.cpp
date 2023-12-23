#include <vector>
#include <algorithm>

#include "helpers.hpp"

using std::vector;
using std::reverse;

double_byte concatenate_bytes(byte b1, byte b2)
{
	return b1 << BITS_PER_BYTE | b2;
}

/**
 * A double_byte is made up of 4 nibbles, which we number from 0 till 3.
 * This function returns a double_byte derived from the given double_byte where
 * the nibbles not in the specified range are set to 0.
 *
 * Example: Let's say the given double_byte contains the value 0xABCD. We can
 * request nibbles of in the range 1 to 2, i.e., 0xBC.
 * get_nibbles_in_range(0xABCD, 1, 2) will return 0x00BC.
 */
double_byte get_nibbles_in_range(double_byte b, int first, int last)
{
	constexpr auto NIBBLES_PER_BYTE = BITS_PER_BYTE / BITS_PER_NIBBLE;
	constexpr auto NIBBLES_PER_DOUBLE_BYTE = 2 * NIBBLES_PER_BYTE;

	assert(last < NIBBLES_PER_DOUBLE_BYTE);
	// shift so that all useless nibbles on the right are discarded
	// bits are useless if they occur after `last`
	b >>= (NIBBLES_PER_DOUBLE_BYTE - last - 1) * BITS_PER_NIBBLE;

	const auto num_nibbles_to_extract = last - first + 1;
	assert(num_nibbles_to_extract <= NIBBLES_PER_DOUBLE_BYTE);
	assert(num_nibbles_to_extract > 0);

	// To set bits not in range to 0, we need a mask.
	// Notice that if we have 0x0ABC and we want to keep the last two nibbles,
	// we can BITAND with 0xFF (which is 16 * 16 - 1).
	unsigned mask = 1 << BITS_PER_NIBBLE;
	for (auto i = 1; i < num_nibbles_to_extract; ++i)
	{
		mask *= 1 << BITS_PER_NIBBLE;
	}

	return b & (mask - 1);
}

vector<int> get_digits(size_t num)
{
	vector<int> digits;
	
	for (; num != 0; num /= 10)
	{
		const auto d = num % 10;
		digits.push_back(d);
	}

	reverse(digits.begin(), digits.end());
	return digits;
}

byte get_most_significant_bit(byte value)
{
	return (value >> (BITS_PER_BYTE - 1)) & 1;
}

byte get_least_significant_bit(byte value)
{
	return value & 1;
}
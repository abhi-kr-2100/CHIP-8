#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include "CHIP-8.hpp"

using std::cerr;
using std::cin;
using std::ifstream;
using std::array;
using std::ios;
using std::vector;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " file\n";
		return 1;
	}

	ifstream rom{ argv[1], ios::binary };
	vector<unsigned char> bytes;
	for (int byte = 0; (byte = rom.get()), !rom.eof(); bytes.push_back(byte))
	{
	}

	array<instruction_t, MAX_NUM_INSTRUCTIONS> program{};
	for (size_t i = 0, j = 0, sz = bytes.size(); i < sz; i += INSTRUCTION_SIZE, ++j)
	{
		instruction_t combined = bytes[i];
		for (size_t j = 1; j < INSTRUCTION_SIZE; ++j)
		{
			combined = combined << sizeof(byte) | bytes[i + j];
		}

		program[j] = combined;
	}

	CHIP_8 machine;
	machine.load_program(program);
	machine.run();

	cerr << "Press enter to exit...";
	cin.get();
}
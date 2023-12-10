#include <iostream>
#include <fstream>
#include <array>

#include "CHIP-8.hpp"

using std::cerr;
using std::cin;
using std::ifstream;
using std::array;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " file\n";
		return 1;
	}

	ifstream file{ argv[1] };
	array<instruction_t, MAX_NUM_INSTRUCTIONS> program{};

	for (size_t i = 0; i < MAX_NUM_INSTRUCTIONS && !file.eof(); ++i)
	{
		file >> program[i];
	}

	CHIP_8 machine;
	machine.load_program(program);
	machine.run();

	cerr << "Press enter to exit...";
	cin.get();
}
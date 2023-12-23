#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include <ftxui/screen/screen.hpp>

#include "CHIP-8.hpp"
#include "helpers.hpp"

using std::cerr;
using std::cin;
using std::ifstream;
using std::array;
using std::ios;
using std::vector;

using ftxui::Screen;
using ftxui::Dimension::Fixed;

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
		byte first = bytes[i];
		byte second = bytes[i + 1];

		instruction_t ins = concatenate_bytes(first, second);
		program[j] = ins;
	}

	CHIP_8 machine;
	machine.load_program(program);

	auto screen
	{ 
		Screen::Create(Fixed(FRAME_BUFFER_WIDTH), Fixed(FRAME_BUFFER_HEIGHT))
	};
	while (machine.run_one())
	{
		for (size_t x = 0; x < FRAME_BUFFER_WIDTH; ++x)
		{
			for (size_t y = 0; y < FRAME_BUFFER_HEIGHT; ++y)
			{
				screen.at(x, y) = machine.get_pixel_at(x, y) ? "*" : " ";
			}
		}
		screen.Print();
	}

	cerr << "Press enter to exit...";
	cin.get();
}
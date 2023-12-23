#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include <SFML/Graphics.hpp>

#include "CHIP-8.hpp"
#include "helpers.hpp"

using std::cerr;
using std::cin;
using std::ifstream;
using std::array;
using std::ios;
using std::vector;

using sf::RenderWindow;
using sf::VideoMode;
using sf::Style::Titlebar;
using sf::Style::Close;
using sf::Event;
using sf::Event::Closed;
using sf::Color;
using sf::Texture;
using sf::Uint8;
using sf::Sprite;

static array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> extract_frame_buffer(const CHIP_8& machine);

template <size_t SCALING_FACTOR>
static Texture load_texture_from_frame_buffer(const array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>& fb);

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

	constexpr auto SCALING_FACTOR = 10;
	auto window = RenderWindow{
		VideoMode{FRAME_BUFFER_WIDTH * SCALING_FACTOR, FRAME_BUFFER_HEIGHT * SCALING_FACTOR},
		"CHIP-8",
		Titlebar | Close
	};
	window.setFramerateLimit(60);
	
	for (bool rom_running = true; window.isOpen(); )
	{
		if (rom_running)
		{
			rom_running = machine.run_one();
		}

		for (Event e; window.pollEvent(e); )
		{
			if (e.type == Closed)
			{
				window.close();
			}
		}

		const auto frame_buffer = extract_frame_buffer(machine);
		const auto screen_texture = load_texture_from_frame_buffer<SCALING_FACTOR>(frame_buffer);

		auto screen_sprite = Sprite{ screen_texture };

		window.clear(Color::Black);
		window.draw(screen_sprite);
		window.display();
	}

	cerr << "Press enter to exit...";
	cin.get();
}

static array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>
extract_frame_buffer(const CHIP_8& machine)
{
	array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> fb{};
	for (size_t x = 0; x < FRAME_BUFFER_WIDTH; ++x)
	{
		for (size_t y = 0; y < FRAME_BUFFER_HEIGHT; ++y)
		{
			fb[x][y] = machine.get_pixel_at(x, y);
		}
	}

	return fb;
}

template <size_t SCALING_FACTOR>
static Texture
load_texture_from_frame_buffer(const array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>& fb)
{
	constexpr auto NUM_CHANNELS = 4;
	constexpr auto OPAQUE = 255;
	constexpr auto BLACK = 0;
	constexpr auto WHITE = 255;

	static auto pixels = new Uint8[FRAME_BUFFER_WIDTH * SCALING_FACTOR * FRAME_BUFFER_HEIGHT * SCALING_FACTOR * NUM_CHANNELS];

	for (size_t x = 0; x < FRAME_BUFFER_WIDTH; ++x)
	{
		for (size_t y = 0; y < FRAME_BUFFER_HEIGHT; ++y)
		{
			const auto is_black = fb[x][y];

			const auto startx = x * SCALING_FACTOR;
			const auto starty = y * SCALING_FACTOR;

			for (size_t i = 0; i < SCALING_FACTOR; ++i)
			{
				for (size_t j = 0; j < SCALING_FACTOR; ++j)
				{
					const auto pixel_offset = ((starty + i) * FRAME_BUFFER_WIDTH * SCALING_FACTOR + (startx + j)) * NUM_CHANNELS;
					pixels[pixel_offset + 3] = OPAQUE;
					pixels[pixel_offset + 0] = pixels[pixel_offset + 1] = pixels[pixel_offset + 2] = is_black ? BLACK : WHITE;
				}
			}
		}
	}

	auto texture = Texture{};
	texture.create(FRAME_BUFFER_WIDTH * SCALING_FACTOR, FRAME_BUFFER_HEIGHT * SCALING_FACTOR);
	texture.update(pixels);

	return texture;
}
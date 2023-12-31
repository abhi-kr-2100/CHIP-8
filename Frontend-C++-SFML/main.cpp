#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>
#include <unordered_map>

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "CHIP-8.hpp"
#include "helpers.hpp"
#include "data-types.hpp"
#include "machine-specs.hpp"
#include "keyboard.hpp"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::cerr;
using std::ifstream;
using std::array;
using std::ios;
using std::vector;
using std::istream;
using std::unordered_map;

using sf::RenderWindow;
using sf::VideoMode;
using sf::Style::Titlebar;
using sf::Style::Close;
using sf::Event;
using sf::Event::Closed;
using sf::Event::KeyPressed;
using sf::Event::KeyReleased;
using sf::Color;
using sf::Texture;
using sf::Uint8;
using sf::Sprite;

const unordered_map<sf::Keyboard::Scancode, Key> KBD_TO_CHIP_8 = {
	{ sf::Keyboard::Scan::X, Key::K0 },
	{ sf::Keyboard::Scan::Num1, Key::K1 },
	{ sf::Keyboard::Scan::Num2, Key::K2 },
	{ sf::Keyboard::Scan::Num3, Key::K3 },
	{ sf::Keyboard::Scan::Q, Key::K4 },
	{ sf::Keyboard::Scan::W, Key::K5 },
	{ sf::Keyboard::Scan::E, Key::K6 },
	{ sf::Keyboard::Scan::A, Key::K7 },
	{ sf::Keyboard::Scan::S, Key::K8 },
	{ sf::Keyboard::Scan::D, Key::K9 },
	{ sf::Keyboard::Scan::Z, Key::KA },
	{ sf::Keyboard::Scan::C, Key::KB },
	{ sf::Keyboard::Scan::Num4, Key::KC },
	{ sf::Keyboard::Scan::R, Key::KD },
	{ sf::Keyboard::Scan::F, Key::KE },
	{ sf::Keyboard::Scan::V, Key::KF },
};

template <size_t SCALING_FACTOR>
static Texture load_texture_from_frame_buffer(const Frame_buffer& fb);

bool redraw_necessary(const Frame_buffer& fb);

template <size_t SCALING_FACTOR>
void redraw_if_necessary(RenderWindow& window, const Frame_buffer& fb);

void load_program_from_stream(istream& stream, CHIP_8& machine);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " file\n";
		return 1;
	}

	ifstream rom{ argv[1], ios::binary };
	CHIP_8 machine;
	load_program_from_stream(rom, machine);

	constexpr auto SCALING_FACTOR = 10;
	auto window = RenderWindow{
		VideoMode{ FRAME_BUFFER_WIDTH * SCALING_FACTOR, FRAME_BUFFER_HEIGHT * SCALING_FACTOR },
		"CHIP-8",
		Titlebar | Close
	};

	auto last_limiter_check_time = system_clock::now();
	for (bool rom_running = true; window.isOpen(); )
	{
		for (Event e; window.pollEvent(e); )
		{
			switch (e.type)
			{
			case Closed:
				window.close();
				break;
			case KeyPressed:
				if (KBD_TO_CHIP_8.find(e.key.scancode) != KBD_TO_CHIP_8.end())
				{
					machine.keyboard.set_key_pressed(KBD_TO_CHIP_8.at(e.key.scancode));
				}
				break;
			case KeyReleased:
				if (KBD_TO_CHIP_8.find(e.key.scancode) != KBD_TO_CHIP_8.end())
				{
					machine.keyboard.set_key_released(KBD_TO_CHIP_8.at(e.key.scancode));
				}
				break;
			}
		}

		const auto curr_time = system_clock::now();

		const auto time_elapsed = curr_time - last_limiter_check_time;
		const auto milliseconds_elapsed = duration_cast<milliseconds>(time_elapsed).count();

		if (milliseconds_elapsed < MILLISECONDS_PER_REFRESH)
		{
			continue;
		}
		last_limiter_check_time = curr_time;

		const auto refreshes_elapsed = milliseconds_elapsed / MILLISECONDS_PER_REFRESH;

		const size_t nins = refreshes_elapsed * INSTRUCTIONS_PER_REFRESH;
		for (size_t i = 0; rom_running && i < nins; ++i)
		{
			rom_running = machine.run_one();
		}

		const size_t timer_decrements_elapsed = refreshes_elapsed * TIMER_DECREMENTS_PER_REFRESH;
		machine.decrement_timers(timer_decrements_elapsed);

		const auto& frame_buffer = machine.get_frame_buffer();
		redraw_if_necessary<SCALING_FACTOR>(window, frame_buffer);
	}
}

bool redraw_necessary(const Frame_buffer& fb)
{
	static Frame_buffer previous_fb{};

	bool necessary = false;
	for (size_t x = 0; x < fb.size(); ++x)
	{
		for (size_t y = 0; y < fb[0].size(); ++y)
		{
			if (previous_fb[x][y] != fb[x][y])
			{
				necessary = true;
			}

			previous_fb[x][y] = fb[x][y];
		}
	}

	return necessary;
}

template <size_t SCALING_FACTOR>
void redraw_if_necessary(RenderWindow& window, const Frame_buffer& fb)
{
	if (!redraw_necessary(fb))
	{
		return;
	}

	const auto screen_texture = load_texture_from_frame_buffer<SCALING_FACTOR>(fb);
	const auto screen_sprite = Sprite{ screen_texture };

	window.clear();
	window.draw(screen_sprite);
	window.display();
}

template <size_t SCALING_FACTOR>
static Texture load_texture_from_frame_buffer(const Frame_buffer& fb)
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
			const bool is_black = fb[x][y];

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

void load_program_from_stream(istream& stream, CHIP_8& machine)
{
	constexpr auto max_num_bytes = MAX_NUM_INSTRUCTIONS * INSTRUCTION_SIZE;
	array<byte, max_num_bytes> bytes{};
	bytes.fill(0);
	for (auto& b : bytes)
	{
		if (!stream)
		{
			break;
		}

		b = stream.get();
	}

	machine.load_program_from_bytes(bytes);
}
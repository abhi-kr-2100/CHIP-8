#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <chrono>

#include <SFML/Graphics.hpp>

#include "CHIP-8.hpp"
#include "helpers.hpp"

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::cerr;
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

using Frame_buffer = array<array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>;

static Frame_buffer extract_frame_buffer(const CHIP_8& machine);

template <size_t SCALING_FACTOR>
static Texture load_texture_from_frame_buffer(const Frame_buffer& fb);

bool redraw_necessary(const Frame_buffer& fb);

template <size_t SCALING_FACTOR>
void redraw_if_necessary(RenderWindow& window, const Frame_buffer& fb);


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
		byte second = (i + 1 < sz) ? bytes[i + 1] : 0x0;

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

	auto start_time = system_clock::now();
	size_t nins_executed = 0, ntimer_updated = 0, nscreen_refreshed = 0;
	
	auto last_limiter_check_time = system_clock::now();
	for (bool rom_running = true; window.isOpen(); )
	{
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
		nins_executed += nins;

		const size_t timer_decrements_elapsed = refreshes_elapsed * TIMER_DECREMENTS_PER_REFRESH;
		machine.decrement_timers(timer_decrements_elapsed);
		ntimer_updated += timer_decrements_elapsed;

		for (Event e; window.pollEvent(e); )
		{
			if (e.type == Closed)
			{
				window.close();
			}
		}

		const auto frame_buffer = extract_frame_buffer(machine);
		redraw_if_necessary<SCALING_FACTOR>(window, frame_buffer);

		nscreen_refreshed += refreshes_elapsed;
	}

	const auto time_elapsed = system_clock::now() - start_time;
	const auto seconds_elapsed = duration_cast<seconds>(time_elapsed).count();
	cerr << "Execution speed: " << (double(nins_executed) / seconds_elapsed) << "\n"
		<< "Timer speed: " << (double(ntimer_updated) / seconds_elapsed) << "\n"
		<< "FPS: " << (double(nscreen_refreshed) / seconds_elapsed) << "\n";
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

static Frame_buffer extract_frame_buffer(const CHIP_8& machine)
{
	Frame_buffer fb{};
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
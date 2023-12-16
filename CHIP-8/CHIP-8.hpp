#pragma once

#include <cstdint>
#include <array>
#include <functional>
#include <map>

#include "data-types.hpp"
#include "font-data.hpp"
#include "machine-specs.hpp"

class CHIP_8
{
public:
	void load_program(const std::array<instruction_t, MAX_NUM_INSTRUCTIONS>& program);
	void run();

	CHIP_8();

	void set_framebuffer_pixel(size_t x, size_t y);
	void unset_framebuffer_pixel(size_t x, size_t y);
	bool get_framebuffer_pixel(size_t x, size_t y) const;

	size_t get_framebuffer_width() const;
	size_t get_framebuffer_height() const;

	double_byte get_pc() const;
	void set_pc(double_byte address);

	size_t get_num_registers() const;
	void set_register(size_t register_, byte value);
	byte get_register(size_t register_) const;

	double_byte get_index_register() const;
	void set_index_register(double_byte val);
private:
	std::array<byte, MEMORY_SIZE> memory;
	std::array<byte, NUM_REGISTERS> registers;
	std::array<double_byte, STACK_SIZE / STACK_ENTRY_SIZE> stack;

	std::array<std::array<bool, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH> frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	std::map<byte, std::function<void(byte, byte, byte, byte, double_byte, CHIP_8&)>>
		executors;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
};
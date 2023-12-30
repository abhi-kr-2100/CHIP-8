#pragma once

#include <array>

#include "data-types.hpp"
#include "font-data.hpp"
#include "machine-specs.hpp"
#include "keyboard.hpp"

class Executor;

class CHIP_8
{
public:
	void load_program(const ROM& program);
	bool run_one();

	const Frame_buffer& get_frame_buffer() const;
	void decrement_timers(byte times);

	Keyboard keyboard;

	CHIP_8();
	~CHIP_8();
private:
	std::array<byte, MEMORY_SIZE> memory;
	std::array<byte, NUM_REGISTERS> registers;
	std::array<double_byte, STACK_SIZE / STACK_ENTRY_SIZE> stack;

	Frame_buffer frame_buffer;

	double_byte pc;
	double_byte index_register;
	byte stack_pointer;

	byte delay_timer;
	byte sound_timer;

	bool is_blocked;

	void load_fonts(double_byte start_location, const decltype(FONT_DATA)& font_data);
	Instruction get_current_instruction() const;

	Executor* executor;

	friend class Executor;

	class Helper
	{
	public:
		static void insert_instruction(CHIP_8& machine, instruction_t ins, double_byte location);
		static Instruction make_instruction_from_bytes(instruction_t bytes);
		static double_byte get_sprite_start_location(byte sprite_number);
	};
};
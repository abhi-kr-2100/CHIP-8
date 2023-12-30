#pragma once

#include <vector>
#include <array>

#include "CHIP-8.hpp"
#include "keyboard-data.hpp"

class Executor
{
public:
	Executor(CHIP_8& machine);
	void execute(const CHIP_8::Instruction& ins);
private:
	CHIP_8& machine;
	const std::array<void(Executor::*)(const CHIP_8::Instruction::Instruction_payload&), 16> executors;

	void category_0(const CHIP_8::Instruction::Instruction_payload& payload);
	void jump(const CHIP_8::Instruction::Instruction_payload& payload);
	void subroutine_call(const CHIP_8::Instruction::Instruction_payload& payload);
	void skip_if_vx_eq_nn(const CHIP_8::Instruction::Instruction_payload& payload);
	void skip_if_vx_neq_nn(const CHIP_8::Instruction::Instruction_payload& payload);
	void skip_if_vx_eq_vy(const CHIP_8::Instruction::Instruction_payload& payload);
	void set_register(const CHIP_8::Instruction::Instruction_payload& payload);
	void inc_reg_by_const(const CHIP_8::Instruction::Instruction_payload& payload);
	void operate_and_assign(const CHIP_8::Instruction::Instruction_payload& payload);
	void skip_if_vx_neq_vy(const CHIP_8::Instruction::Instruction_payload& payload);
	void set_index_register(const CHIP_8::Instruction::Instruction_payload& payload);
	void jump_with_offset(const CHIP_8::Instruction::Instruction_payload& payload);
	void set_random(const CHIP_8::Instruction::Instruction_payload& payload);
	void draw(const CHIP_8::Instruction::Instruction_payload& payload);
	void skip_cond_key(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_F(const CHIP_8::Instruction::Instruction_payload& payload);

	class Helper
	{
	public:
		static void clear_screen(CHIP_8& machine);
		static void return_(CHIP_8& machine);

		static std::vector<Key> get_pressed_keys(const CHIP_8& machine);
	};
};
#pragma once

#include <vector>
#include <array>

#include "CHIP-8.hpp"
#include "keyboard.hpp"
#include "data-types.hpp"

class Executor
{
public:
	Executor(CHIP_8& machine);
	void execute(const Instruction& ins);
private:
	CHIP_8& machine;
	const std::array<void(Executor::*)(const Instruction::Instruction_payload&), 16> executors;

	void category_0(const Instruction::Instruction_payload& payload);
	void jump(const Instruction::Instruction_payload& payload);
	void subroutine_call(const Instruction::Instruction_payload& payload);
	void skip_if_vx_eq_nn(const Instruction::Instruction_payload& payload);
	void skip_if_vx_neq_nn(const Instruction::Instruction_payload& payload);
	void skip_if_vx_eq_vy(const Instruction::Instruction_payload& payload);
	void set_register(const Instruction::Instruction_payload& payload);
	void inc_reg_by_const(const Instruction::Instruction_payload& payload);
	void operate_and_assign(const Instruction::Instruction_payload& payload);
	void skip_if_vx_neq_vy(const Instruction::Instruction_payload& payload);
	void set_index_register(const Instruction::Instruction_payload& payload);
	void jump_with_offset(const Instruction::Instruction_payload& payload);
	void set_random(const Instruction::Instruction_payload& payload);
	void draw(const Instruction::Instruction_payload& payload);
	void skip_cond_key(const Instruction::Instruction_payload& payload);
	void category_F(const Instruction::Instruction_payload& payload);

	class Helper
	{
	public:
		static void clear_screen(CHIP_8& machine);
		static void return_(CHIP_8& machine);
	};
};
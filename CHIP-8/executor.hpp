#pragma once

#include "CHIP-8.hpp"

class Executor
{
public:
	Executor(CHIP_8& machine);
	void execute(const CHIP_8::Instruction& ins);
private:
	CHIP_8& machine;
	const std::array<void(Executor::*)(const CHIP_8::Instruction::Instruction_payload&), 16> executors;

	void category_0(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_1(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_2(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_3(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_4(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_5(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_6(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_7(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_8(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_9(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_A(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_B(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_C(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_D(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_E(const CHIP_8::Instruction::Instruction_payload& payload);
	void category_F(const CHIP_8::Instruction::Instruction_payload& payload);
};
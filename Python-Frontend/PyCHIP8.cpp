#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include "CHIP-8.hpp"
#include "keyboard.hpp"
#include "machine-specs.hpp"
#include "debugger.hpp"

namespace py = pybind11;

PYBIND11_MODULE(PyCHIP8, m)
{
	m.doc() = "CHIP-8 emulator library";

	py::class_<CHIP_8>(m, "CHIP_8")
		.def(py::init())
		.def("load_program", &CHIP_8::load_program)
		.def("load_program_from_bytes", &CHIP_8::load_program_from_bytes)
		.def("run_one", &CHIP_8::run_one)
		.def_property_readonly("frame_buffer", &CHIP_8::get_frame_buffer)
		.def("decrement_timers", &CHIP_8::decrement_timers)
		.def_readonly("keyboard", &CHIP_8::keyboard);

	py::class_<Debugger>(m, "Debugger")
		.def(py::init<CHIP_8&>())
		.def("run_one", &Debugger::run_one)
		.def("go_back_one", &Debugger::go_back_one)
		.def_property_readonly("memory", &Debugger::get_memory)
		.def("set_memory_byte", &Debugger::set_memory_byte)
		.def("get_register", &Debugger::get_register)
		.def("set_register", &Debugger::set_register)
		.def_property_readonly("registers", &Debugger::get_registers)
		.def_property("pc", &Debugger::get_pc, &Debugger::set_pc)
		.def_property("index_register", &Debugger::get_index_register,
					  &Debugger::set_index_register)
		.def("on_exec", &Debugger::on_exec)
		.def("run_one_without_callback", &Debugger::run_one_without_callback)
		.def("go_back_one_without_callback", &Debugger::go_back_one_without_callback);

	py::class_<Keyboard>(m, "Keyboard")
		.def(py::init())
		.def("set_key_pressed", &Keyboard::set_key_pressed)
		.def("set_key_released", &Keyboard::set_key_released)
		.def("is_key_pressed", &Keyboard::is_key_pressed);

	py::class_<Instruction>(m, "Instruction")
		.def_readonly("raw", &Instruction::raw_instruction)
		.def_readonly("category", &Instruction::category);

	py::enum_<Key>(m, "Key")
		.value("K0", Key::K0)
		.value("K1", Key::K1)
		.value("K2", Key::K2)
		.value("K3", Key::K3)
		.value("K4", Key::K4)
		.value("K5", Key::K5)
		.value("K6", Key::K6)
		.value("K7", Key::K7)
		.value("K8", Key::K8)
		.value("K9", Key::K9)
		.value("KA", Key::KA)
		.value("KB", Key::KB)
		.value("KC", Key::KC)
		.value("KD", Key::KD)
		.value("KE", Key::KE)
		.value("KF", Key::KF)
		.value("NONE", Key::NONE)
		.export_values();

	py::enum_<Execution_event>(m, "ExecutionEvent")
		.value("RUN_ONE", Execution_event::RUN_ONE)
		.value("GO_BACK_ONE", Execution_event::GO_BACK_ONE)
		.export_values();

	m.attr("MILLISECONDS_PER_REFRESH") = MILLISECONDS_PER_REFRESH;
	m.attr("INSTRUCTIONS_PER_REFRESH") = INSTRUCTIONS_PER_REFRESH;
	m.attr("TIMER_DECREMENTS_PER_REFRESH") = TIMER_DECREMENTS_PER_REFRESH;
	m.attr("MAX_NUM_INSTURCTIONS") = MAX_NUM_INSTRUCTIONS;
	m.attr("INSTRUCTION_SIZE") = INSTRUCTION_SIZE;
}
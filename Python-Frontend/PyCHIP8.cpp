#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "CHIP-8.hpp"
#include "keyboard.hpp"
#include <machine-specs.hpp>

namespace py = pybind11;

PYBIND11_MODULE(PyCHIP8, m)
{
	m.doc() = "CHIP-8 emulator library";

	py::class_<CHIP_8>(m, "CHIP_8")
		.def(py::init())
		.def("load_program", &CHIP_8::load_program)
		.def("run_one", &CHIP_8::run_one)
		.def_property_readonly("frame_buffer", &CHIP_8::get_frame_buffer)
		.def("decrement_timers", &CHIP_8::decrement_timers)
		.def_readonly("keyboard", &CHIP_8::keyboard);

	py::class_<Keyboard>(m, "Keyboard")
		.def(py::init())
		.def("set_key_pressed", &Keyboard::set_key_pressed)
		.def("set_key_released", &Keyboard::set_key_released)
		.def("is_key_pressed", &Keyboard::is_key_pressed);

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

	m.attr("MILLISECONDS_PER_REFRESH") = MILLISECONDS_PER_REFRESH;
	m.attr("INSTRUCTIONS_PER_REFRESH") = INSTRUCTIONS_PER_REFRESH;
	m.attr("TIMER_DECREMENTS_PER_REFRESH") = TIMER_DECREMENTS_PER_REFRESH;
}
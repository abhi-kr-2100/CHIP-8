#include <pybind11/pybind11.h>

namespace py = pybind11;

int add(int i, int j)
{
	return i + j;
}

PYBIND11_MODULE(PyCHIP8, m)
{
	m.doc() = "CHIP-8 emulator library";
	m.def("add", &add, "Add two given integers.");
}
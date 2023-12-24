#pragma once

#include <cstdint>
#include <array>

#include "machine-specs.hpp"

using byte = std::uint8_t;
using double_byte = std::uint16_t;
using instruction_t = double_byte;

using Frame_buffer = std::array<std::array<byte, FRAME_BUFFER_HEIGHT>, FRAME_BUFFER_WIDTH>;
#pragma once

#include <array>
#include <cstdint>

/**
 * CHIP-8 had prebuilt sprites to represent single hexadecimal digits: from 0 to F.
 * This header provides an encoding of these digits.
 * 
 * Source: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#font
 */

constexpr auto FONT_CHAR_SIZE = 5 /* bytes */;
constexpr auto FONT_NUM_CHARS = 16; /* 0x0 to 0xF */

constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_0 = { 0xF0, 0x90, 0x90, 0x90, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_1 = { 0x20, 0x60, 0x20, 0x20, 0x70, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_2 = { 0xF0, 0x10, 0xF0, 0x80, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_3 = { 0xF0, 0x10, 0xF0, 0x10, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_4 = { 0x90, 0x90, 0xF0, 0x10, 0x10, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_5 = { 0xF0, 0x80, 0xF0, 0x10, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_6 = { 0xF0, 0x80, 0xF0, 0x90, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_7 = { 0xF0, 0x10, 0x20, 0x40, 0x40, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_8 = { 0xF0, 0x90, 0xF0, 0x90, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_9 = { 0xF0, 0x90, 0xF0, 0x10, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_A = { 0xF0, 0x90, 0xF0, 0x90, 0x90, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_B = { 0xE0, 0x90, 0xE0, 0x90, 0xE0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_C = { 0xF0, 0x80, 0x80, 0x80, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_D = { 0xE0, 0x90, 0x90, 0x90, 0xE0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_E = { 0xF0, 0x80, 0xF0, 0x80, 0xF0, };
constexpr std::array<std::uint8_t, FONT_CHAR_SIZE> FONT_DATA_F = { 0xF0, 0x80, 0xF0, 0x80, 0x80, };

constexpr std::array<decltype(FONT_DATA_0), FONT_NUM_CHARS> FONT_DATA = {
	FONT_DATA_0,
	FONT_DATA_1,
	FONT_DATA_2,
	FONT_DATA_3,
	FONT_DATA_4,
	FONT_DATA_5,
	FONT_DATA_6,
	FONT_DATA_7,
	FONT_DATA_8,
	FONT_DATA_9,
	FONT_DATA_A,
	FONT_DATA_B,
	FONT_DATA_C,
	FONT_DATA_D,
	FONT_DATA_E,
	FONT_DATA_F,
};
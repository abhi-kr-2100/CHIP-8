#pragma once

#include <unordered_map>

#include <SFML/Window.hpp>

enum class Key
{
	NONE = -1, K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, KA, KB, KC, KD, KE, KF
};

const std::unordered_map<Key, sf::Keyboard::Scancode> CHIP_8_TO_KBD = {
	{ Key::K0, sf::Keyboard::Scan::X },
	{ Key::K1, sf::Keyboard::Scan::Num1 },
	{ Key::K2, sf::Keyboard::Scan::Num2 },
	{ Key::K3, sf::Keyboard::Scan::Num3 },
	{ Key::K4, sf::Keyboard::Scan::Q },
	{ Key::K5, sf::Keyboard::Scan::W },
	{ Key::K6, sf::Keyboard::Scan::E },
	{ Key::K7, sf::Keyboard::Scan::A },
	{ Key::K8, sf::Keyboard::Scan::S },
	{ Key::K9, sf::Keyboard::Scan::D },
	{ Key::KA, sf::Keyboard::Scan::Z },
	{ Key::KB, sf::Keyboard::Scan::C },
	{ Key::KC, sf::Keyboard::Scan::Num4 },
	{ Key::KD, sf::Keyboard::Scan::R },
	{ Key::KE, sf::Keyboard::Scan::F },
	{ Key::KF, sf::Keyboard::Scan::V },
};
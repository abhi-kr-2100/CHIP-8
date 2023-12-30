#pragma once

#include <unordered_map>

#include "keyboard-data.hpp"

class Keyboard
{
public:
	void set_key_pressed(Key k);
	void set_key_released(Key k);

	bool is_key_pressed(Key k) const;

	Keyboard();
private:
	std::unordered_map<Key, bool> statuses;
};
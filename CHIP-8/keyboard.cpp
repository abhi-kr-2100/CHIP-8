#include "keyboard.hpp"

Keyboard::Keyboard()
{
	for (auto k = Key::K0; k <= Key::KF; k = static_cast<Key>((int)k + 1))
	{
		statuses[k] = false;
	}
}

void Keyboard::set_key_pressed(Key k)
{
	statuses[k] = true;
}

void Keyboard::set_key_released(Key k)
{
	statuses[k] = false;
}

bool Keyboard::is_key_pressed(Key k) const
{
	return statuses.at(k);
}
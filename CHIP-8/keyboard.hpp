#pragma once

#include <unordered_map>

enum class Key
{
	K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, KA, KB, KC, KD, KE, KF, NONE
};

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
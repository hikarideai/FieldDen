#pragma once
#include <SFML\Graphics.hpp>

extern sf::Font g_font, g_small_font;

template<typename T>
int sign(T n) {
	if (n < 0) return -1;
	else return 1;
}


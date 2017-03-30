#pragma once
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>

extern sf::Font g_font, g_small_font;
extern std::vector<std::pair<std::string, std::string>> history;
extern int history_index;

template<typename T>
int sign(T n) {
	if (n < 0) return -1;
	else return 1;
}


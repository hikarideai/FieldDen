#pragma once
#include <map>
#include <fstream>
#include <string>
#include <SFML/System/String.hpp>

class Language
{
public:
	Language();
	Language(std::string path);

	void loadFromFile(std::string path);

	sf::String operator[](sf::String ind);
private:
	std::map<sf::String, sf::String> dict;
};

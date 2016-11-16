#include "Language.hpp"
#include <iostream>
#include <locale>
#include <codecvt>

Language::Language()
{
	setlocale(LC_ALL, "Russian");
}

Language::Language(std::string path)
{
	setlocale(LC_ALL, "Russian");
	loadFromFile(path);
}

void Language::loadFromFile(std::string path)
{
	dict.clear();

	std::cout << "\nLoading up the dictionary from " << path << '\n';

	setlocale(LC_ALL, "Russian");

	std::ifstream f(path);

	std::string trans, real;
	while (!f.eof())
	{
		f >> real;
		f >> trans;
		std::cout << real << " -> " << trans << '\n';
		dict[sf::String(real)] = sf::String(trans);
	}
	std::cout << "Dictionary loaded.\n\n";
	f.close();
}

sf::String Language::operator[](sf::String ind)
{
	std::cout << "Requested " << ind.toAnsiString() << " -> Returned " << dict[ind].toAnsiString() << '\n';
	return dict[ind];
}

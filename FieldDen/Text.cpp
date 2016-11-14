#include "Text.hpp"

Text::Text(sf::Font &font, std::string text, uint16_t size, sf::Vector2f pos) {
    text_.setCharacterSize(size);

    text_.setString("()/");
    text_.setOrigin(text_.getGlobalBounds().left, text_.getGlobalBounds().top);
    text_.setString(text);

    text_.setPosition(pos);
    text_.setColor(sf::Color::Black);
    text_.setFont(font);
    text_.setStyle(sf::Text::Regular);
    visible_ = true;
}


EVENT_TYPE Text::check(sf::Event& event, sf::Window& relative) {
	return NOTHING;
}
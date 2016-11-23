#include "Button.hpp"
#include <iostream>

void Button::updateText() {
    text_.setString(content_);

    sf::FloatRect temp_bounds = text_.getLocalBounds();
    body_.setSize(sf::Vector2f(std::max(temp_bounds.width + 3 * size_ / 8, 3 * size_ / 4), 3 * size_ / 4));

    sf::Vector2f body_pos = body_.getPosition();
    text_.setPosition(body_pos.x + size_ / 4, body_pos.y + size_ / 8);
}

Button::Button(sf::Font &font, const sf::String &label, sf::Vector2f pos, unsigned int size) : content_(label) {
    text_.setFont(font);
    text_.setColor(sf::Color::Black);
    text_.setCharacterSize(size / 2);
    text_.setStyle(sf::Text::Regular);

    text_.setString("()/");
    text_.setOrigin(text_.getGlobalBounds().left, text_.getGlobalBounds().top);
    text_.setString(content_);

    size_ = (float)size;

    body_.setPosition(pos);
    body_.setOrigin(0, 0);
    text_.setPosition(sf::Vector2f(pos.x + size_ / 8, pos.y + size_ / 8));
    body_.setFillColor(sf::Color(225, 225, 225, 200));
    updateText();
    visible_ = true;
    hover_ = false;
}


EVENT_TYPE Button::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;

    //Mouse iteraction
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative);

    //std::cout << "Mouse pos (" << mouse_pos.x << ", " << mouse_pos.y << ")\n";
    auto isMouseIn = [&]() {
        return (body_.getGlobalBounds().left <= mouse_pos.x
            && mouse_pos.x <= body_.getGlobalBounds().left + body_.getGlobalBounds().width
            && body_.getGlobalBounds().top <= mouse_pos.y
            && mouse_pos.y <= body_.getGlobalBounds().top + body_.getGlobalBounds().height);
    };
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.key.code == sf::Mouse::Left
            && isMouseIn()) {
            std::cout << "Button clicked\n";
            click();
            click_func();
            return CLICK;
        }
        return NOTHING;
    case sf::Event::MouseButtonReleased:
        if (event.key.code == sf::Mouse::Left
            && isMouseIn()) {
            std::cout << "Button clicked\n";
            body_.setFillColor(sf::Color(146, 187, 252, 200));
            return UNCLICK;
        }
        return NOTHING;
    default:
        if (isMouseIn()) {
            mouseOver();
            mo();
            return MOUSE_OVER;
        }
        else {
            mouseNotOver();
            mno();
            return MOUSE_NOT_OVER;
        }
    }
}

inline void Button::mouseNotOver()
{
    if (hover_) {
        hover_ = false;
        body_.setFillColor(sf::Color(225, 225, 225, 200));
    }
}

inline void Button::mouseOver()
{
    if (!hover_) {
        hover_ = true;
        body_.setFillColor(sf::Color(146, 187, 252, 200)); //Light blue
    }
}

inline void Button::click()
{
    body_.setFillColor(sf::Color(108, 164, 252, 200));     //darkBlue
}

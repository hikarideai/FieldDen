#include "ImageButton.hpp"

ImageButton::ImageButton(std::string path, sf::Vector2f pos) {
    if (!image_.loadFromFile(path)) {
        std::cout << "ERROR:\tUnable to load button's glyph from " << path << "\n";
    }

    glyph_.setTexture(image_);
    glyph_.setPosition(pos);
    glyph_.setOrigin(0, 0);

    body_.setPosition(pos);
    body_.setOrigin(0, 0);
    body_.setFillColor(sf::Color::Transparent);
    body_.setSize(sf::Vector2f((float)image_.getSize().x, (float)image_.getSize().y));
    visible_ = true;
    hover_ = false;
    default_ = sf::Color::Transparent;
}


EVENT_TYPE ImageButton::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;

    //Mouse utility
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative);
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
            std::cout << "INFO:\tButton clicked\n";
            click();
            click_func();
            return CLICK;
        }
        return NOTHING;
    case sf::Event::MouseButtonReleased:
        if (event.key.code == sf::Mouse::Left
            && isMouseIn()) {
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

inline void ImageButton::mouseNotOver()
{
    if (hover_) {
        hover_ = false;
        body_.setFillColor(default_);
    }
}

inline void ImageButton::mouseOver()
{
    if (!hover_) {
        hover_ = true;
        body_.setFillColor(sf::Color(146, 187, 252, 200)); //Light blue
    }
}

inline void ImageButton::click()
{
    body_.setFillColor(sf::Color(108, 164, 252, 200));     //darkBlue
}
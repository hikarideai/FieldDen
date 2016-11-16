#pragma once
#include "Widget.hpp"
#include "Globals.hpp"
#include <iostream>

class Text : public Widget {
public:
    Text(sf::Font &font = g_font, sf::String text = "", uint16_t size = 14, sf::Vector2f pos = sf::Vector2f(0,0));

    void setCharSize(uint16_t size)
    {
        text_.setCharacterSize(size);
    }

    //From Widget
    sf::Vector2f size() {
        return sf::Vector2f(text_.getGlobalBounds().width, text_.getGlobalBounds().height);
    }
    sf::Vector2f pos() {
        return text_.getPosition();
    }
    void setPos(sf::Vector2f pos) {
        text_.setPosition(pos);
    }
    Widget* clone() const {
        return new Text(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    void setContent(sf::String content) {
        text_.setString(content);
    }
    std::string content() { return text_.getString(); }

    //Graphics
    void show() { visible_ = true; }
    void hide() { visible_ = false; }
    bool visible() { return visible_; }

private:
    sf::Text text_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(text_);
    }
};

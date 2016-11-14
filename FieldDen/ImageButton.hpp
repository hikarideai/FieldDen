#pragma once
#include "Widget.hpp"
#include <iostream>

class ImageButton : public Widget {
public:
    ImageButton(std::string path, sf::Vector2f pos);

    //From Widget
    sf::Vector2f size() {
        return sf::Vector2f(body_.getGlobalBounds().width, body_.getGlobalBounds().height);
    }
    sf::Vector2f pos() {
        return body_.getPosition();
    }
    void setPos(sf::Vector2f pos) {
        body_.setPosition(pos);
        glyph_.setPosition(pos);
    }
    Widget* clone() const {
        return new ImageButton(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    void ifMouseOver(std::function<void()> f) { mo = f; }
    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void onClick(std::function<void()> f) { click_func = f; }

private:
    sf::Texture image_;
    sf::Sprite glyph_;
    sf::RectangleShape body_;
    sf::Color default_;

    bool hover_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(body_);
        target.draw(glyph_);        
    }

    //Events
    void mouseNotOver();
    std::function<void()> mno = []() {};   //Mouse Not Over

    void mouseOver();
    std::function<void()> mo = []() {};

    void click();
    std::function<void()> click_func = []() {};
};
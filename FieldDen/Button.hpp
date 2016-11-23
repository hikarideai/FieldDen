#pragma once
#include "Widget.hpp"

class Button : public Widget {
public:
    Button(sf::Font &font, const sf::String &label, sf::Vector2f pos, unsigned int size = 32);

    //From Widget
    sf::Vector2f size() {
        return sf::Vector2f(body_.getGlobalBounds().width, body_.getGlobalBounds().height);
    }
    sf::Vector2f pos() {
        return body_.getPosition();
    }
    void setPos(sf::Vector2f pos) {
        body_.setPosition(pos);
        text_.setPosition(sf::Vector2f(pos.x + size_ / 8, pos.y + size_ / 8));
    }
    Widget* clone() const {
        return new Button(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    std::string content() { return content_.toAnsiString(); }
    void content(sf::String label) {
        content_ = label;
        updateText();
    }

    void ifMouseOver(std::function<void()> f) { mo = f; }
    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void onClick(std::function<void()> f) { click_func = f; }

private:
    sf::Text text_;
    sf::String content_;

    sf::RectangleShape body_;
    float size_;

    bool hover_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(body_);
        target.draw(text_);
    }

    void updateText();

    //Events
    void mouseNotOver();
    std::function<void()> mno = []() {};

    void mouseOver();
    std::function<void()> mo = []() {};

    void click();
    std::function<void()> click_func = []() {};
};
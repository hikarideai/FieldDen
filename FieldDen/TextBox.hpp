#pragma once
#include "Widget.hpp"
#include <iostream>
#include "Globals.hpp"

class TextBox : public Widget {
public:
    TextBox(sf::Font &font = g_font, sf::Vector2f pos = sf::Vector2f(0,0), unsigned int size = 32);

    //From Widget
    sf::Vector2f size() { 
        return sf::Vector2f(body_.getGlobalBounds().width, body_.getGlobalBounds().height); 
    }
    sf::Vector2f pos() { 
        return body_.getPosition();
    }
    void setPos(sf::Vector2f pos) { 
        body_.setPosition(pos);
        text_.setPosition(sf::Vector2f(0, 0));
    }
    Widget* clone() const {
        return new TextBox(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    std::string content() { return content_; }

    void focus() { edit_ = true; }
    void onlyNumbers() { l_ = '0'; h_ = '9'; }
    void setContent(std::string new_cont) {
        content_ = new_cont;
        updateText();
    }

    //Events
    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void ifMouseOver(std::function<void()> f) { mo = f; }
    void onClick(std::function<void()> f) { click_func = f; }
    void onUnclick(std::function<void()> f) { unclick_func = f; }
    void ifTextDeleted(std::function<void()> f) { txd = f; }
    void onTextInput(std::function<void()> f) { txi = f; }
    void ifTextChanged(std::function<void()> f) { txd = f; txi = f; }

private:
    sf::Text text_;
    std::string content_;

    sf::RectangleShape body_;
    float size_;

    bool edit_, hover_, only_numbers_;
    char l_, h_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(body_);
        target.draw(text_);
    }

    void updateText();

    //Events
    void mouseNotOver();
    std::function<void()> mno = []() {}; //Mouse Not Over

    void mouseOver();
    std::function<void()> mo = []() {};   //Mouse Over

    void click();
    std::function<void()> click_func = []() {};

    void unclick();
    std::function<void()> unclick_func = []() {};

    void textDeleted();
    std::function<void()> txd = []() {};  //TeXt Deleted
    std::function<void()> txi = []() {};  //text input
};

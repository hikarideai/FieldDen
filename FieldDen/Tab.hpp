#pragma once
#include "Widget.hpp"
#include <iostream>

#define TEXT_SIZE 12
#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()[]{}0123456789!@#$%^&*/-+,.?|"

class Tab : public Widget {
public:
    Tab()
    {
        visible_ = true;
        hover_ = false;
        active_ = false;
        default_ = sf::Color::Transparent;
    }
    Tab(std::string path, int size = 32, sf::Vector2f pos = sf::Vector2f(0, 0));

    //From Widget
    sf::Vector2f size() {
        return sf::Vector2f(body_.getGlobalBounds().width, body_.getGlobalBounds().height);
    }
    sf::Vector2f pos() {
        return body_.getPosition();
    }
    void setPos(sf::Vector2f pos) {
        body_.setPosition(pos);
        label_.setPosition(pos.x + (body_.getSize().x-label_.getLocalBounds().width)/2, pos.y + body_.getSize().y/4);
    }
    Widget* clone() const {
        return new Tab(*this);
    }

	EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    //Graphics
    void show() { visible_ = true; }
    void hide() { visible_ = false; }
    bool visible() { return visible_; }
    void deactivate();
    void activate();
    bool state() { return active_; }

    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void ifMouseOver(std::function<void()> f) { mo = f; }
    void onClick(std::function<void()> f) { click_func = f; }
    void onActivation(std::function<void()> f) { act = f; }
    void onDeactivation(std::function<void()> f) { dact = f; }

private:
    sf::Text label_;
    sf::RectangleShape body_;
    sf::Color default_;

    bool hover_, active_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(body_);
        target.draw(label_);
    }

    //Events
    void mouseNotOver();
    std::function<void()> mno = [](){};

    void mouseOver();
    std::function<void()> mo = [](){};
    
    bool click();
    std::function<void()> click_func = []() {};

    std::function<void()> act = []() {};
    std::function<void()> dact = []() {};
};
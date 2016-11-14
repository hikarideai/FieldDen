#pragma once
#include "SFML/Graphics.hpp"
#include "EventType.hpp"
#include <functional>

class Widget : public sf::Drawable {
public:
    virtual sf::Vector2f size() = 0;
    virtual sf::Vector2f pos() = 0;
    virtual void setPos(sf::Vector2f pos) = 0;
    virtual EVENT_TYPE check(sf::Event& event, sf::Window& relative) = 0;
    virtual Widget* clone() const = 0;

    virtual ~Widget() {}

    virtual void show() { visible_ = true; }
    virtual void hide() { visible_ = false; }
    virtual bool visible() { return visible_; }

protected:
    bool visible_ = false;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const = 0;
};
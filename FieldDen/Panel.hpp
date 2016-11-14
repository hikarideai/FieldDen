#pragma once
#include "Widget.hpp"
#include <vector>

class Panel : public Widget {
public:
    Panel() { pos_ = sf::Vector2f(); visible_ = true; }
    explicit Panel(sf::Vector2f pos) : pos_(pos) { visible_ = true; }
    void init(sf::Vector2f pos) {
        pos_ = pos;
        visible_ = true;
    }

    void add(Widget* element);

    sf::Vector2f size() { return sf::Vector2f(0, 0); }
    sf::Vector2f pos() { return pos_; }
    void setPos(sf::Vector2f pos) { pos_ = pos; }
	EVENT_TYPE check(sf::Event& event, sf::Window& relative);
    Widget* clone() const {
        return new Panel(*this);
    }
private:
    sf::Vector2f pos_;
    std::vector<Widget*> elements_;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
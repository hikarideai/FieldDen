#include "Panel.hpp"
#include <iostream>

void Panel::add(Widget* element) {
    element->setPos(element->pos() + pos_);
    elements_.push_back(element);
}

EVENT_TYPE Panel::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;
    for (auto &i : elements_) {
        i->check(event, relative);
    }
	return DONE;
}

void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!visible_) return;
    sf::Vector2f tpos;
    for (auto &i : elements_) {
        target.draw(*i);
    }
}
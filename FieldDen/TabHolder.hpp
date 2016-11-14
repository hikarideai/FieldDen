#pragma once
#include "Tab.hpp"
#include <deque>

enum Order { ORDER_HORIZONTALY, ORDER_VERTICALY };

class TabHolder : public Widget {
public:
    TabHolder() { ind = 1; ord_ = ORDER_HORIZONTALY; }
    TabHolder(sf::Vector2f pos, Order ord = ORDER_HORIZONTALY) : pos_(pos), ord_(ord) { ind = 1; visible_ = true; }
    void init(sf::Vector2f pos, Order ord = ORDER_HORIZONTALY) {
        pos_ = pos;
        ord_ = ord;
        visible_ = true;
        ind = 1;
    }
    
    void add(Tab* tab);

    sf::Vector2f size() {
        return size_;
    }
    sf::Vector2f pos() {
        return pos_;
    }
    void setPos(sf::Vector2f pos) {
        pos_ = pos;
    }
    Widget* clone() const {
        return new TabHolder(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

private:
    size_t ind;
    sf::Vector2f pos_;
    Order ord_;
    sf::Vector2f size_;
    std::deque<Tab*> strip_;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};



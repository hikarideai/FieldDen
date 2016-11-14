#include "TabHolder.hpp"

//FIXME: Sometimes events skipped
EVENT_TYPE TabHolder::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;
    //Mouse iteraction
    auto mouse_pos = sf::Mouse::getPosition(relative);

    //std::cout << "Mouse pos (" << mouse_pos.x << ", " << mouse_pos.y << ")\n";
    auto isMouseIn = [&]() {
        return (pos_.x <= mouse_pos.x
            && mouse_pos.x <= pos_.x + size_.x
            && pos_.y <= mouse_pos.y
            && mouse_pos.y <= pos_.y + size_.y);
    };

    //if (isMouseIn()) {        
    auto found = false;
    for (size_t j = 0; j < strip_.size(); j++) {
        if (strip_[j]->state()) {
            ind = j;
            found = true;
            break;
        }
    }
    if (!found) for (auto i : strip_) {
        i->check(event, relative);
    }
    else for (size_t j = 0; j < strip_.size(); j++) {
        strip_[j]->check(event, relative);
        if (ind != j && strip_[j]->state()) {
            strip_[ind]->deactivate();
        }
        
    }
}

void TabHolder::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!visible_) return;
    sf::Vector2f pos = pos_;
    for (auto i : strip_) {
        i->setPos(pos);
        target.draw(*i);
        if (ord_ == ORDER_HORIZONTALY)
            pos.x += i->size().x+4;
        else
            pos.y += i->size().y+4;
    }
}

void TabHolder::add(Tab* tab) {
    if (ord_ == ORDER_HORIZONTALY)
        size_ = sf::Vector2f(size_.x + tab->size().x+4, std::max(size_.y, tab->size().y+4));
    else
        size_ = sf::Vector2f(std::max(size_.x, tab->size().x+4), size_.y + tab->size().y+4);
    strip_.push_back(tab);
}

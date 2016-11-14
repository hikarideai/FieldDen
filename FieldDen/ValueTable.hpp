#pragma once
#include "Slider.hpp"
#include "Text.hpp"
#include <deque>
#include <memory>
#include <list>

#define SLIDER_PADDING 8

class ValueTable : public Widget {
public:
    ValueTable() { ind = 1; }
    ValueTable(sf::Vector2f pos) : pos_(pos) { ind = 1; visible_ = true; }
    void init(sf::Vector2f pos) {
        pos_ = pos;    visible_ = true;
        ind = 1;
    }

    void add(std::string id, Slider* slider);
    std::map<std::string, double> get();


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
        return new ValueTable(*this);
    }

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);

    void clear() {
        for (auto i : strip_) {
            free(i);
        }
        strip_.clear();
        size_ = sf::Vector2f(0,0);
    }

	void genFrom(std::deque<std::string> &table);

    void setSample(Slider a) { sample_ = a; }
	void setShift(int x, int y)
    {
		shift_x = x;
		shift_y = y;
    }

private:
    Slider sample_;
    size_t ind;
    sf::Vector2f pos_;
    sf::Vector2f size_;
    std::list<Slider*> strip_;

	int shift_x = 0, shift_y = 0;

	void realign();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

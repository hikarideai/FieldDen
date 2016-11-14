#pragma once
#include "Widget.hpp"
#include "EventType.hpp"
#include "TextBox.hpp"
#include "Text.hpp"

#define  SLIDER_SHIFT 8

struct Vector2d {
    Vector2d() { x = 0; y = 0; }
    Vector2d(double a, double b) {
        x = a;
        y = b;
    }
    double x, y;
};

class Slider : public Widget {
public:
    //Slider(const Slider &);
    Slider(Vector2d bounds = Vector2d(-10, 10), sf::Vector2f pos = sf::Vector2f(), double width = 200);

    void setName(std::string s)
    {
        name_ = s;
        label->setContent(name_ + ": 0");
    }
    std::string getName() const { return name_; }
	Vector2d getRange() const { return Vector2d(min_, max_); }

    //From Widget
    sf::Vector2f size() {
        return sf::Vector2f(body_.getGlobalBounds().width, body_.getSize().y+label->size().y+ SLIDER_SHIFT);
    }
    sf::Vector2f pos() {
        return body_.getPosition();
    }
    void setRange(Vector2d r) {
        max_ = r.y;
        min_ = r.x;
        calcConstans();
    }
    void setPos(sf::Vector2f pos) {
        slider_.setPosition(slider_.getPosition().x + std::abs(body_.getPosition().x - pos.x), pos.y+label->size().y+ SLIDER_SHIFT);
        body_.setPosition(pos.x, pos.y + label->size().y + SLIDER_SHIFT);
        label->setPos(pos);
		pos_ = pos;

        calcConstans();
    }
    Widget* clone() const {
        return new Slider(*this);
    }
	
	Slider* copy() {
		Slider *clone = new Slider(Vector2d(min_, max_), pos_, size_);
		clone->setValue(value_);
		/*std::function<void()> mno = []() {};

    void mouseOver();
    std::function<void()> mo = []() {};

    void click();
    std::function<void()> click_func = []() {};
    std::function<void()> btnrel = []() {};
    std::function<void()> mvd = []() {};
    std::function<void()> cust_val = []() {};
	ifMouseOver(std::function<void()> f) { mo = f; }
    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void onClick(std::function<void()> f) { click_func = f; }
    void ifSliderMoved(std::function<void()> f) { mvd = f; }
    void ifValueChanged(std::function<void()> f) { btnrel = f; cust_val = f; }
    void onApply(std::function<void()> f) { btnrel = f; }
    void ifCustomValueApplied(std::function<void()> f) { cust_val = f; }
	*/
		clone->ifMouseOver(mo);
		clone->ifMouseNotOver(mno);
		clone->onClick(click_func);
		clone->ifSliderMoved(mvd);
		clone->onApply(btnrel);
		clone->ifCustomValueApplied(cust_val);
		return clone;
	}

    EVENT_TYPE check(sf::Event& event, sf::Window& relative);
    double getValue() const;
    void setValue(double val);

    ~Slider() {
        //delete setter_;
    }


    void ifMouseOver(std::function<void()> f) { mo = f; }
    void ifMouseNotOver(std::function<void()> f) { mno = f; }
    void onClick(std::function<void()> f) { click_func = f; }
    void ifSliderMoved(std::function<void()> f) { mvd = f; }
    void ifValueChanged(std::function<void()> f) { btnrel = f; cust_val = f; }
    void onApply(std::function<void()> f) { btnrel = f; }
    void ifCustomValueApplied(std::function<void()> f) { cust_val = f; }

private:
	sf::Vector2f pos_;
    TextBox *setter_ = nullptr;
    Text *label = nullptr;
    std::string name_;

    sf::RectangleShape body_;
    sf::RectangleShape slider_;
    double size_;

    bool hover_, edit_, deep_;
    sf::Vector2i previous_mp;

	//0 = k*min + b
	//Slider.x = k*max + b
	double value_;
	double a_, b_, c_;
    double max_, min_;
	double calcValue() const;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if (!visible_) return;
        target.draw(body_);
        target.draw(slider_);
        target.draw(*label);
        target.draw(*setter_);
    }
    void calcConstans() {
		// (y1-y2)x+(x2-x1)y+(x1*y2-x2*y1)=0
		a_ = min_ - max_;
        b_ = body_.getSize().x;
		//x1 = 0
		c_ = -body_.getSize().x*min_;
    }

    //Events
    void mouseNotOver();
    std::function<void()> mno = []() {};

    void mouseOver();
    std::function<void()> mo = []() {};

    void click();
    std::function<void()> click_func = []() {};
    std::function<void()> btnrel = []() {};
    std::function<void()> mvd = []() {};
    std::function<void()> cust_val = []() {};
};

#include "Slider.hpp"
//g_font
#include "Globals.hpp"
//Debug
#include <iostream>

#define SENSITIVITY 8

Slider::Slider(Vector2d bounds, sf::Vector2f pos, double width) {
    size_ = width;
	pos_ = pos;
    setter_ = new TextBox();
    label = new Text();
    label->setPos(pos);
    label->setCharSize(12);

    //Initialize slider thumb
    slider_.setSize(sf::Vector2f(6,6));
    slider_.setOrigin(slider_.getSize().x/2, slider_.getSize().y / 2-1);
    slider_.setPosition(pos.x, pos.y+label->size().y+ SLIDER_SHIFT);

    //Initialize body
    body_.setPosition(pos);
    body_.setOrigin(0, 0);
    body_.setSize(sf::Vector2f(size_, 2));

    //Intialize label
    name_ = "Unnamed";
    label->setContent(name_ + ": 0");

    //Initialize color theme
    slider_.setFillColor(sf::Color(63, 81, 181, 255));
    slider_.setOutlineColor(sf::Color(63, 81, 181, 100));
    body_.setFillColor(sf::Color(190, 190, 190, 200));
    
    //Line equation initialization
    max_ = bounds.y;
    min_ = bounds.x;
    calcConstans();
    setValue(0.0);
    
    visible_ = true;
    edit_ = false;
    hover_ = false;
    deep_ = false;

    //Let user be able to determine slider value
    setter_->onUnclick([&]() {
        if (setter_->content() != "") {
			try {
				this->setValue(std::stold(setter_->content()));
			} catch(std::invalid_argument)
			{
				std::cout << "Error> Wrong input\n";
			}
		}
        setter_->hide();
        setter_->setContent("");
        deep_ = false;
        label->setContent(name_ + ": " + std::to_string(getValue()));
        cust_val();
    });
    setter_->hide();
    setter_->onlyNumbers();
}


EVENT_TYPE Slider::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;
    if (deep_) {
        if (setter_->check(event, relative) == UNCLICK) 
			return CUSTOM_CHANGE;
    }

    //Mouse iteraction
    auto mouse_pos = sf::Mouse::getPosition(relative);
    auto isMouseIn = [&]() {
        return (body_.getPosition().x <= mouse_pos.x
            && mouse_pos.x <= body_.getPosition().x + body_.getSize().x
            && slider_.getPosition().y-6 <= mouse_pos.y
            && mouse_pos.y <= slider_.getPosition().y + 6);
    };

    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.key.code == sf::Mouse::Left
            && isMouseIn()) {
            if (!edit_) {
                previous_mp = mouse_pos;
                edit_ = true;
                click();
                click_func();
				return CLICK;
            }            
        }
        else if (event.key.code == sf::Mouse::Right) {
            edit_ = false;
            if (!deep_ && isMouseIn()) {
                deep_ = true;
                hover_ = true;
                setter_->setPos(sf::Vector2f(mouse_pos.x, mouse_pos.y-setter_->size().y));
                setter_->show();
                setter_->focus();
                std::cout << "show";
                mo();
				return MOUSE_OVER;
            }
            else {
                hover_ = true;
                deep_ = false;
                mouseNotOver();
                mno();
				return MOUSE_NOT_OVER;
            }
        }
        return NOTHING;
    case sf::Event::MouseButtonReleased:
        if (event.key.code == sf::Mouse::Left) {
			if (!edit_) return NOTHING;
        	btnrel();
            edit_ = false;
            if (isMouseIn()) {
                hover_ = false;
                mouseOver();
                mo();
				return MOUSE_OVER;
            }
            else {
                hover_ = true;
                mouseNotOver();
                mno();
				return MOUSE_NOT_OVER;
            }
        }
		return NOTHING;
    case sf::Event::MouseMoved:
        if (edit_) {
            sf::Vector2f dt;
            
			int dir = mouse_pos.x - previous_mp.x, sense = std::abs(slider_.getPosition().y - mouse_pos.y);
			if (std::abs(sense)-SENSITIVITY > 0) {
				dt.x = dir*(9 / std::pow(std::abs(sense - SENSITIVITY)+3,2));
				std::cout << dt.x << '\n';
			}
			else dt.x = mouse_pos.x - previous_mp.x;
            dt.y = 0 ;

			//std::cout << dt.x << '\n';

            slider_.setPosition(slider_.getPosition() + dt);
            if (slider_.getPosition().x < body_.getPosition().x) {
                slider_.setPosition(body_.getPosition().x, slider_.getPosition().y);
            }
            else if (body_.getPosition().x + body_.getSize().x < slider_.getPosition().x) {
                slider_.setPosition(body_.getPosition().x + body_.getSize().x, slider_.getPosition().y);
            }

			value_ = calcValue();
            label->setContent(name_ + ": " + std::to_string(value_));

            previous_mp = mouse_pos;
            mvd();
            return MOUSE_MOVED;
        }
    default:
        if (isMouseIn()) {
            mouseOver();
            mo();
            return MOUSE_OVER;
        }
        else {
            if (!edit_) {
                mouseNotOver();
                mno();
                return MOUSE_NOT_OVER;
            }
        }
    }
}

inline void Slider::mouseNotOver()
{
    if (hover_) {
        hover_ = false;
        body_.setFillColor(sf::Color(190, 190, 190, 200));
        slider_.setOutlineThickness(0);
        slider_.setSize(sf::Vector2f(6, 6));
        slider_.setOrigin(slider_.getSize().x / 2, slider_.getSize().y / 2 - 1);
    }
}

inline void Slider::mouseOver()
{
    if (!hover_) {
        hover_ = true;
        body_.setFillColor(sf::Color(143, 143, 143, 200)); //Light blue
        slider_.setOutlineThickness(3);
        slider_.setSize(sf::Vector2f(6, 6));
        slider_.setOrigin(slider_.getSize().x / 2, slider_.getSize().y / 2 - 1);
    }
}

void Slider::click()
{
    body_.setFillColor(sf::Color(143, 143, 143, 200));     //darkBlue
    slider_.setOutlineThickness(0);
    slider_.setSize(sf::Vector2f(12, 12));
    slider_.setOrigin(slider_.getSize().x / 2, slider_.getSize().y / 2 - 1);
}

double Slider::getValue() const {
	return value_;
}

double Slider::calcValue() const {
    return (-(a_*(slider_.getPosition().x-body_.getPosition().x)+c_)/b_);
}

void Slider::setValue(double val) {
    if (val > max_) {
        max_ = val;
        calcConstans();
    }
    else if (min_ > val) {
        min_ = val;
        calcConstans();
    }
    auto new_pos = slider_.getPosition();
	new_pos.x = -(b_*val + c_) / a_ + body_.getPosition().x;
    slider_.setPosition(new_pos);
	value_ = val;
    label->setContent(name_ + ": " + std::to_string(value_));
	cust_val();
}

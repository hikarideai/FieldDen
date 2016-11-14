#include "Tab.hpp"
#include "Globals.hpp"

Tab::Tab(std::string title, int size, sf::Vector2f pos) {	
	label_.setCharacterSize(TEXT_SIZE);
	label_.setFont(std::ref(g_font));
	label_.setColor(sf::Color::Black);
	label_.setString(title);

	label_.setString(ALPHABET);
	sf::FloatRect labelRect = label_.getGlobalBounds();
	label_.setOrigin(labelRect.left, labelRect.top);
	label_.setString(title);

    body_.setPosition(pos);
    body_.setOrigin(0, 0);
    body_.setFillColor(sf::Color::Transparent);
    body_.setSize(sf::Vector2f(label_.getLocalBounds().left + label_.getLocalBounds().width+8, size));
	label_.setPosition(sf::Vector2f(pos.x + body_.getSize().x / 2, pos.y + body_.getSize().y/2));

    visible_ = true;
    hover_ = false;
    active_ = false;
    default_ = sf::Color::Transparent;
}

EVENT_TYPE Tab::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;

    //Mouse utility
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative);
    auto isMouseIn = [&]() {
        return (body_.getGlobalBounds().left <= mouse_pos.x
            && mouse_pos.x <= body_.getGlobalBounds().left + body_.getGlobalBounds().width
            && body_.getGlobalBounds().top <= mouse_pos.y
            && mouse_pos.y <= body_.getGlobalBounds().top + body_.getGlobalBounds().height);
    };

    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        if (event.key.code == sf::Mouse::Left
            && isMouseIn()) {
            if (active_) {
                std::cout << "INFO:\tTab deactivated\n";
                click_func();
                deactivate();
                dact();
				return TAB_DEACTIVATED;
            }
            else {
                std::cout << "INFO:\tTab activated\n";
                click_func();
                activate();
                act();
				return TAB_ACTIVATED;
            }
        }
        return NOTHING;
    default:
        if (isMouseIn()) {
            mouseOver();
            mo();
			return MOUSE_OVER;
        }
        else {
            mouseNotOver();
            mno();
			return MOUSE_NOT_OVER;
        }
    }
}

inline void Tab::mouseNotOver()
{
    if (hover_ && !active_) {
        hover_ = false;
        body_.setFillColor(default_);
    }
}

inline void Tab::mouseOver()
{
    if (!hover_ && !active_) {
        hover_ = true;
        body_.setFillColor(sf::Color(146, 187, 252, 200)); //Light blue
    }
}

inline bool Tab::click()
{
    if (!active_) {
        active_ = true;
        body_.setFillColor(sf::Color(108, 164, 252, 200));     //darkBlue
        act();
        return true;
    }
    else {
        active_ = false;
        body_.setFillColor(sf::Color(146, 187, 252, 200)); //Light blue
        dact();
        return false;
    }
}

void Tab::deactivate() {
    active_ = false;
    dact();
    body_.setFillColor(sf::Color::Transparent); //Light blue
}

void Tab::activate() {
    active_ = true;
    act();
    body_.setFillColor(sf::Color(108, 164, 252, 200));
}
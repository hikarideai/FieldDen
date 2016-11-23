#include "TextBox.hpp"

TextBox::TextBox(sf::Font &font, sf::Vector2f pos, unsigned int size) {
	text_.setString(content_);
	text_.setCharacterSize(size / 2);
	text_.setColor(sf::Color::Black);
	text_.setFont(font);
	text_.setStyle(sf::Text::Regular);

	text_.setString("()/");
	text_.setOrigin(text_.getGlobalBounds().left, text_.getGlobalBounds().top);
	text_.setString("");

	size_ = size;

	body_.setPosition(pos);
	body_.setOrigin(0, 0);
	text_.setPosition(sf::Vector2f(pos.x + size_ / 8, pos.y + size_ / 8));
	body_.setFillColor(sf::Color(225, 225, 225, 200));
	// body_.setOutlineColor(sf::Color(200, 200, 200, 200));
	body_.setSize(sf::Vector2f(3 * size_ / 4, 3 * size_ / 4));
	edit_ = false;
	hover_ = false;
	visible_ = true;
	only_numbers_ = false;

	l_ = 32;
	h_ = 127;
}

EVENT_TYPE TextBox::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;
    //Mouse iteraction
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative);

    //std::cout << "Mouse pos (" << mouse_pos.x << ", " << mouse_pos.y << ")\n";
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
            click();
            click_func();
			return CLICK;
        }
        else {
            unclick();
            unclick_func();
			return UNCLICK;
        }
		return NOTHING;

    case sf::Event::KeyPressed:
        if (!edit_) return NOTHING;
        if (event.key.code == sf::Keyboard::BackSpace || event.key.code == sf::Keyboard::Delete) {
            if (content_.size() <= 0) return NOTHING;
            textDeleted();
            txd();
        }
		return TEXT_DELETED;
    case sf::Event::KeyReleased:
        if (!edit_) return NOTHING;
        if (event.key.code == sf::Keyboard::Return) {
            unclick();
            unclick_func();
        }
		return UNCLICK;
    case sf::Event::TextEntered:
        if (!edit_) return NOTHING;
        if (event.text.unicode == '.' || (text_.getString() == "" && event.text.unicode == '-') || (l_ <= event.text.unicode && event.text.unicode <= h_)) {
            content_ += static_cast<char>(event.text.unicode);
            updateText();
            txi();
        }
		return TEXT_ADDED;

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

void TextBox::updateText() {
    text_.setString(content_);

    sf::FloatRect temp_bounds = text_.getLocalBounds();
    body_.setSize(sf::Vector2f(std::max(temp_bounds.width + 3 * size_ / 8, 3 * size_ / 4), 3 * size_ / 4));

    sf::Vector2f body_pos = body_.getPosition();
    text_.setPosition(body_pos.x + size_ / 4, body_pos.y + size_ / 8);
}

inline void TextBox::mouseNotOver()
{
    if (!edit_ && hover_) {
        hover_ = false;
        body_.setFillColor(sf::Color(225, 225, 225, 200));
    }
}

inline void TextBox::mouseOver()
{
    if (!hover_ && !edit_) {
        hover_ = true;
        body_.setFillColor(sf::Color(146, 187, 252, 200)); //Light blue
    }
}

inline void TextBox::click()
{
    if (!edit_) {
        edit_ = true;
        body_.setFillColor(sf::Color(108, 164, 252, 200));    //darkBlue
    }
}

inline void TextBox::unclick()
{
    if (edit_) {
        edit_ = false;
        hover_ = false;
        body_.setFillColor(sf::Color(225, 225, 225, 200));
    }
}

inline void TextBox::textDeleted()
{
    content_.pop_back();
    updateText();
}
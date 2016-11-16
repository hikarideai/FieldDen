#include "ValueTable.hpp"


EVENT_TYPE ValueTable::check(sf::Event& event, sf::Window& relative) {
    if (!visible_) return NOTHING;
    //Mouse iteraction
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(relative);

    //std::cout << "Mouse pos (" << mouse_pos.x << ", " << mouse_pos.y << ")\n";
    auto isMouseIn = [&]() {
        return (pos_.x <= mouse_pos.x
            && mouse_pos.x <= pos_.x + size_.x
            && pos_.y <= mouse_pos.y
            && mouse_pos.y <= pos_.y + size_.y);
    };

    for (auto i : strip_) {
       i->check(event, relative);
    }
}

void ValueTable::realign()
{
	size_t n = 0;
	for(auto &i :strip_)
	{
		i->setPos(sf::Vector2f(pos_.x + shift_x, pos_.y + shift_y + (sample_.size().y + SLIDER_PADDING)*(n + 1)));
		n++;
	}
}

void ValueTable::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (!visible_) return;
    for (auto i : strip_) {
        target.draw(*i);
    }
}

//FIXME: Wrong positioning
void ValueTable::add(std::string id, Slider* slider) {
    size_.y += slider->size().y+8;
    
    size_.x = std::max(size_.x, slider->size().x);

    slider->setPos(sf::Vector2f(pos_.x+shift_x, pos_.y+size_.y+shift_y));
    slider->setName(id);
    strip_.push_back(slider);
}

std::map<std::string, double> ValueTable::get() {
    std::map<std::string, double> var_table;
    for (auto &i : strip_) {
        var_table[i->getName()] = i->getValue();
    }
    return var_table;
}

void ValueTable::genFrom(std::deque<std::string> &table) {
	std::cout << "ParTable> -----------------------------\nVariable Table\tParameter Table\n";
	size_t i = 0, last = strip_.size();
	for (auto it = strip_.begin(); it != strip_.end(); ++it, ++i)
	{
		std::cout << (i >= table.size() ? "              " : table[i]) << "\t" << (*it)->getName().toAnsiString() << "\n";
	}
	for (; i < table.size(); i++)
	{
		std::cout << table[i] << '\n';
	}
	std::cout << "-----------------------------\n";

	i = 0; last = strip_.size();
	for (auto it = strip_.begin(); it != strip_.end(); ++it) {
		if (i < table.size() && table[i] != (*it)->getName()) {
			std::cout << i << ") table[" << i << "] = " << table[i] << "\n ";
			std::cout << "it = " << (*it)->getName().toAnsiString() << "\n ";

			auto *new_slider = sample_.copy();
			new_slider->setPos(sf::Vector2f(pos_.x + shift_x, pos_.y + shift_y + (sample_.pos().y + SLIDER_PADDING)*(i + 1)));
			new_slider->setName(table[i]);
			*it = new_slider;
			i++;
			std::cout << "UPDATE. Y = " << new_slider->pos().y <<"\n";
		}
		else if (i >= table.size())
		{
			std::cout << "it = " << (*it)->getName().toAnsiString() << "\n ";
			strip_.erase(it, strip_.end());
			std::cout << "REACHED table END\n";
			break;
		}
		else
		{
			std::cout << "SKIP\n";
			i++;
		}
	}
	for (; i < table.size(); i++)
	{
		std::cout << i << ") table[" << i << "] = " << table[i] << "\n ";
		auto *new_slider = sample_.copy();
		add(table[i], new_slider);
		std::cout << "NEW SLIDER\n";
	}

	size_.y = (sample_.size().y + 8)*strip_.size();
	std::cout << "New size:" << size_.y << '\n';
	realign();

	std::cout << "\n-----------------------------\n";
}
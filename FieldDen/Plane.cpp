#include "Plane.hpp"
#include <iostream>

void Plane::push(sf::Vector2f vert)
{/*
	//while (drawing_) {}
	push_mutex_.lock();
	//MAY BE WARNING: map's int may not be 0 as default
	vertices_.push_back(vert);
	push_mutex_.unlock();
	if (!insertFragment(vert))
			return;
	need_update_ = true;*/
	//std::cout << '(' << vert.x << ' ' << vert.y << ") ";
	push_mutex_.lock();
	vertices_array_.append(sf::Vertex(vert, sf::Color::Black));
	push_mutex_.unlock();
	need_update_ = true;
}

void Plane::clear()
{
	vertices_array_.clear();
	vertices_.clear();
	fragments_.clear();
	max_density = 0;
	need_update_ = true;
}

void Plane::setStartColor(HSLColor color)
{
	start_colour_ = color;
	need_update_ = true;
}

void Plane::setEndColor(HSLColor color)
{
	end_colour_ = color;
	need_update_ = true;
}

void Plane::setTransparency(sf::Uint8 val)
{
	opacity_ = val;
	need_update_ = true;
}

void Plane::setSize(double w, double h)
{
	size_.x = w;
	size_.y = h;
	texture_.create(w, h);
	plot_.setTexture(texture_);
	need_update_ = true;
}

void Plane::setPosition(double x, double y)
{
	pos_.x = x;
	pos_.y = y;
	need_update_ = true;
}

void Plane::setCentrePosition(sf::Vector2i pos)
{
	centre_ = { int(pos_.x) + pos.x, int(pos_.y) + pos.y };
	need_update_ = true;
}

void Plane::setCentrePosition(int x, int y)
{
	centre_.x = int(pos_.x) + x;
	centre_.y = int(pos_.y) + y;
}

void Plane::saveAsImage(std::string path) const
{
	texture_.copyToImage().saveToFile(path);
}

HSLColor& Plane::startColour()
{
	return start_colour_;
}

HSLColor& Plane::endColour()
{
	return end_colour_;
}

void Plane::setZoom(double val)
{
	zoom = val;
	need_update_ = true;
	/*
	push_mutex_.lock();

	fragments_.clear();
	max_density = 0;

	sf::Clock clock;
	long double av = 0; long long tot = 0;

	for (auto i : vertices_) {
		if (!insertFragment(i)) {
			continue;
		}
	}
	push_mutex_.unlock();
	need_update_ = true;*/
}

void Plane::shiftX(int val)
{
	shift_x += val;
	need_update_ = true;
}

void Plane::shiftY(int val)
{
	shift_y += val;
	need_update_ = true;
}

void Plane::draw(sf::RenderWindow& w)
{
	if (need_update_)
		updateTexture();
	w.draw(plot_);
}

HSLColor Plane::interpolate(double state) const
{
	HSLColor output;
	output.hue = (end_colour_.hue - start_colour_.hue) * state + start_colour_.hue;
	output.sat = (end_colour_.sat - start_colour_.sat) * state + start_colour_.sat;
	output.lum = (end_colour_.lum - start_colour_.lum) * state + start_colour_.lum;
	output.alp = (255 - opacity_) * state + opacity_;
	return output;
}

bool Plane::isPixelVxisible(int x, int y) const
{
	if (x > size_.x || x < 0 ||
		y > size_.y || y < 0 )
		return false;
	return true;
}

sf::Vector2i Plane::absolute(const sf::Vector2f &c) const
{
	sf::Vector2i r;
	r.x = int(centre_.x) + c.x;
	r.y = int(centre_.y) - c.y;
	return r;
}

sf::Vector2f Plane::absolutef(sf::Vector2f c) const
{
	c.x = int(centre_.x) + c.x*zoom;
	c.y = int(centre_.y) - c.y*zoom;
	return c;
}

bool Plane::insertFragment(sf::Vector2f p)
{
	p.x *= zoom; p.y *= zoom;
	if (!isPixelVxisible(absolute(p).x, absolute(p).y))
		return false;

	sf::Vector2i pixel(std::round(p.x), std::round(p.y));
	auto pixel_pos = fragments_.insert(std::make_pair(pixel, 1));
	pixel_pos.first->second++; //FIXME: Strange error occurs
	if (pixel_pos.first->second > max_density) {
		max_density = pixel_pos.first->second;
	}
	return true;
}

//HEAP CORRUPTION's AT THIS POINT
void Plane::updateTexture()
{
	long long width = size_.x, height = size_.y;
	if (width <= 0 || height <= 0)
		return;
	
	long long pixels_size = width * height * 4;
	sf::Uint8 *pixels = new sf::Uint8[pixels_size];

	for (auto i = 0; i < width*height * 4; i++) 
		pixels[i] = 0;

	drawing_ = true;

	sf::Color temp;
	long long limit = 0, col = 0;
	fragments_.clear();

	push_mutex_.lock();
	for (size_t i = 0; i < vertices_array_.getVertexCount(); i++)
	{
		int tx = int(centre_.x) + vertices_array_[i].position.x*zoom+shift_x, ty = int(centre_.y) - vertices_array_[i].position.y*zoom + shift_y;
		
		if (!isPixelVxisible(tx, ty))
			continue;
		if ((tx + ty*width) * 4 + 3 >= pixels_size)
			continue;
		if (pixels[(tx + ty*width) * 4] >= 255)
			continue;

		pixels[(tx + ty*width) * 4]++;
		if (limit < 255 && pixels[(tx + ty*width) * 4] > limit)
			limit = pixels[(tx + ty*width) * 4];		
	}

	for (auto i = 0, den = 0; i < pixels_size; i+=4)
	{
		den = pixels[i];
		//if (i.second > limit) std::cout << "WRONG(" << i.second << ">" << limit << ")\n";
		if (den == 0) temp = sf::Color::White;
		else temp = interpolate(double(den) / limit).toRGB();
		pixels[i] = temp.r;
		pixels[i + 1] = temp.g;
		pixels[i + 2] = temp.b;
		pixels[i + 3] = (den+opacity_ > 255 ? 255 : den+opacity_);
	}

	push_mutex_.unlock();
	//std::cout << "Average density: " << average_den / total_den << "    Average interpolations state: " << average_inter/total_inter << "\n";

	texture_.update(pixels);
	plot_.setTexture(texture_, true);

	need_update_ = false;
	delete[] pixels;
}

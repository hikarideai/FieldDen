#pragma once
#include <stdio.h>
#include <vector>
#include <map>
#include <thread>
#include "SFML/Graphics.hpp"
#include <mutex>
#include <list>

struct HSLColor
{
	int hue; double sat, lum, alp;
	HSLColor() : hue(0), sat(0), lum(0), alp(0) {}
	HSLColor(int a, double b, double c, double d = 255)
		: hue(a), sat(b), lum(c), alp(d) {}
	sf::Color toRGB()
	{
		int h = hue % 360 + (hue < 0) * 360;
		double s = sat, l = lum,
			m2 = l + (l < 0.5 ? l : 1 - l)*s,
			m1 = 2 * l - m2;
		return sf::Color(
			hsl2rgb(h >= 240 ? h-240 : h+120, m1, m2),
			hsl2rgb(h,m1,m2),
			hsl2rgb(h < 120 ? h + 240 : h -120, m1, m2),
			alp
		);
	}
	int hsl2rgb(int h, double m1, double m2)
	{
		return (h < 60 ? m1 + (m2-m1)*h/60
			: h < 180 ? m2
			: h < 240 ? m1 + (m2-m1)*(240-h) / 60
			: m1) * 255;
	}
};

struct VectorComparator
{
	bool operator() (sf::Vector2i lhs, sf::Vector2i rhs) const
	{
		if (lhs.x < rhs.x)
		{
			return true;
		}
		if (rhs.x < lhs.x)
		{
			return false;
		}
		if (lhs.y < rhs.y)
		{
			return true;
		}
		if (rhs.y < lhs.y)
		{
			return false;
		}
		return false;
	}
};

class Plane {
public:
	Plane()
	{
		drawing_ = false;
		pos_ = sf::Vector2f(0, 0);
		size_ = sf::Vector2f(0, 0);
		centre_ = sf::Vector2i(0, 0);
		start_colour_ = HSLColor(200, 0.75, 0.77);
		end_colour_ = HSLColor(360, 0.85, 0.63);
		need_update_= true;
	}

	Plane(const sf::Vector2f& pos, const sf::Vector2f& size)
		: pos_(pos),
		  size_(size)
	{
		need_update_ = true;
	}

	void push(sf::Vector2f vert);
	void updateTexture();
	void clear();

	void setStartColor(HSLColor color);
	void setEndColor(HSLColor color);
	void setTransparency(sf::Uint8 val);
	void setSize(double w, double h);
	void setPosition(double x, double y);
	// Relatively to plain's position (pos_)
	void setCentrePosition(sf::Vector2i pos);
	void setCentrePosition(int x, int y);

	void saveAsImage(std::string path) const;

	HSLColor& startColour();
	HSLColor& endColour();

	//TODO: Vertex point size
	//void setPointSize(double val);

	//TODO: Vertices manipulation
	void setZoom(double val);
	void shiftX(int val);
	void shiftY(int val);
	void realign();

	void update()
	{
		need_update_ = true;
	}
	void draw(sf::RenderWindow &w);

private:
	//Pos_ defines actual position(at the top-left corner)
	sf::Vector2f pos_;
	sf::Vector2f size_;

	// Decart coordinate system centre
	sf::Vector2i centre_;

	sf::Uint8 opacity_ = 255; long long max_density = 0;
	HSLColor start_colour_, end_colour_;

	//TODO: Make them work
	double zoom = 1;
	double shift_x = 0, shift_y = 0;

	std::vector<sf::Vector2f> vertices_;
	std::map<sf::Vector2i, long long, VectorComparator> fragments_;
	sf::VertexArray vertices_array_;
	//Optimization try
	//std::list<std::pair<int, int>, long long> new_fragments_;

	std::recursive_mutex push_mutex_;
	
	bool need_update_, drawing_;
	sf::Texture texture_;
	sf::Sprite plot_;

	HSLColor interpolate(double state) const;
	bool isPixelVxisible(int x, int y) const;
	sf::Vector2i absolute(const sf::Vector2f &c) const;
	sf::Vector2f absolutef(sf::Vector2f c) const;
	bool insertFragment(sf::Vector2f p);
};
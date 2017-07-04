#pragma once

#include <algorithm>
#include <memory>

class Color
{
public:
	float r, g, b;
	Color() :r(0.0), g(0.0), b(0.0){};
	Color(const float &f) : r(f), g(f), b(f){}
	Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b){}
	Color(const Color &c) :r(c.r), g(c.g), b(c.b){};
	
	float length2() const { return r * r + g * g + b * b; }
	float length() const { return std::sqrt(length2()); }

	Color& operator=(const Color &c);

	Color operator+(const Color &c) const;
	Color& operator+=(const Color &c);

	Color operator-(const Color &c) const;
	Color& operator-=(const Color &c);

	Color operator*(float f) const;
	Color& operator*=(float f);
	Color operator*(const Color &c) const;
	Color& operator*=(const Color &c);

	Color operator/(float f) const;
	Color operator/(const Color &c) const;
	Color& operator/=(float f);
	Color& operator/=(const Color &c);
	

	/*CONDITIONS*/
	bool operator==(const Color &other) const;
	bool operator!=(const Color &other) const;

	Color& clamp(float min = 0.0, float max = 1.0);
	bool isBlack() const;
	float luminance() const;

	/*STATIC*/
	static Color clamp(const Color &c, float min = 0.0, float max = 1.0);
	static Color gamma(const Color &c, float gamma = 2.2f);
	
};

inline Color& Color::operator = (const Color &c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	return *this;
}

inline Color Color::operator+(const Color &c) const
{
	return Color(r + c.r, g + c.g, b + c.b);
}

inline Color& Color::operator+=(const Color &c)
{
	r += c.r;
	g += c.g;
	b += c.b;
	return *this;
}

inline Color Color::operator-(const Color &c) const
{
	return Color(r - c.r, g - c.g, b - c.b);
}

inline Color& Color::operator-=(const Color &c)
{
	r -= c.r;
	g -= c.g;
	b -= c.b;
	return *this;
}

inline Color Color::operator*(float f) const
{
	return Color(r * f, g * f, b * f);
}

inline Color& Color::operator*=(float f)
{
	r *= f;
	g *= f;
	b *= f;
	return *this;
}

inline Color Color::operator*(const Color &c) const
{
	return Color(r * c.r, g * c.g, b * c.b);
}

inline Color& Color::operator*=(const Color &c)
{
	r *= c.r;
	g *= c.g;
	b *= c.b;
	return *this;
}

inline Color Color::operator/(float f) const
{
	float inv = 1.0f / f;
	return Color(r * inv, g * inv, b * inv);
}

inline Color Color::operator/(const Color &c) const
{
	return Color(r / c.r, g / c.g, b / c.b);
}

inline Color& Color::operator/=(float f)
{
	float inv = 1.f / f;
	r *= inv;
	g *= inv;
	b *= inv;
	return *this;
}

inline Color& Color::operator/=(const Color &c)
{
	r /= c.r;
	g /= c.g;
	b /= c.b;
	return *this;
}

inline Color operator*(float f, const Color &c)
{
	return c * f;
}

//global minus operator

inline Color operator-(const Color &c1, const Color &c2)
{
	return Color(c1.r - c2.r, c1.g - c2.g, c1.b - c2.b);
}

inline bool Color::operator==(const Color &other) const
{
	return r == other.r && g == other.g && b == other.b;
}
inline bool Color::operator!=(const Color &other) const
{
	return !(*this == other);
}

inline Color& Color::clamp(float min, float max)
{
	r = std::max(min, std::min(max, r));
	g = std::max(min, std::min(max, g));
	b = std::max(min, std::min(max, b));
	return *this;
}

inline bool Color::isBlack() const
{
	return (r == 0.f && b == 0.f && b == 0.f);
}

inline float Color::luminance() const
{
	return 0.212671f * r + 0.715160f * g + 0.072169f * b;
}

inline Color Color::clamp(const Color &c, float min, float max)
{
	float r, g, b;
	r = std::max(min, std::min(max, c.r));
	g = std::max(min, std::min(max, c.g));
	b = std::max(min, std::min(max, c.b));
	return Color(r, g, b);
}

inline Color Color::gamma(const Color &c, float gamma)
{
	float g = 1.0f / gamma;
	return Color(std::max(0.0f, std::min(1.0f, std::pow(c.r, g))),
		std::max(0.0f, std::min(1.0f, std::pow(c.g, g))),
		std::max(0.0f, std::min(1.0f, std::pow(c.b, g))));
}

typedef std::unique_ptr<Color[]> ColorPtr;

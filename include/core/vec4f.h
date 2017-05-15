#ifndef VEC4F_H
#define VEC4F_H

#include <algorithm>
#include <iostream>

class vec4f
{
public:
	float x, y, z, w;
	vec4f() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	vec4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	friend std::ostream& operator<<(std::ostream &o, const vec4f &v);
};

inline std::ostream& operator<<(std::ostream &o, const vec4f &v)
{
	o << "vec4f( " << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
	return o;
}

#endif //VEC4F_H
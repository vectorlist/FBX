#pragma once

#include <vector>
#include <memory>
#include <assert.h>
#include <quaternion.h>
#include <vec3f.h>

class Key
{
public:
	Key(float time) : mTime(time){}
	virtual~Key() {};
	
	float mTime;
};

class VectorKey : public Key
{
public:
	VectorKey() : Key(0) {}

	VectorKey(float x, float y, float z, float time)
		: Key(time)
	{
		mVector.x = x;
		mVector.y = y;
		mVector.z = z;
	}

	VectorKey(const vec3f &other, float time)
		: Key(time)
	{
		mVector = other;
	}
	vec3f mVector;
};

class QuatKey : public Key
{
public:
	QuatKey() : Key(0) {}

	QuatKey(float x, float  y, float z, float w, float time)
		: Key(time)
	{
		mQaut.v.x = x;
		mQaut.v.y = y;
		mQaut.v.z = z;
		mQaut.w   = w;
	}

	QuatKey(const Quaternion &other, float time)
		: Key(time)
	{
		mQaut = other;
	}

	Quaternion mQaut;
};


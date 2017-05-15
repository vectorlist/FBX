#pragma once

#include <vec3f.h>
#include <vec2f.h>



typedef struct FBXVertex_T
{
	vec3f pos;
	vec3f normal;
	vec2f st;

}FBXVertex;

typedef struct MeshVertex_T
{
	vec3f pos;
	vec3f ooo;
	vec3f normal;
	vec2f st;

}MeshVertex;

template<typename T>
inline int getPosOffset()
{
	return offsetof(T, T::pos);
}

template<typename T>
inline int getNormalOffset()
{
	return offsetof(T, T::normal);
}

template<typename T>
inline int getStOffset()
{
	return offsetof(T, T::st);
}
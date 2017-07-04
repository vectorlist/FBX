#pragma once

#include <config.h>
#include <vec2f.h>
#include <vec3f.h>

class Face
{
public:
	void			SetVertexIndex(int facePointIndex, int vertexIndex);
	void			SetNormal(int facePointIndex, const vec3f& normal);
	void			SetST(int facePointIndex, const vec2f& st);

	unsigned int	GetVertexIndex(int facePointIndex);
	vec3f&			GetNormal(int facePointIndex);
	vec2f&			GetST(int facePointIndex);

private:
	unsigned int	mVerticeIndex[FACE_COMPONENT_NUM];
	vec3f			mNormals[FACE_COMPONENT_NUM];
	vec2f			mSTs[FACE_COMPONENT_NUM];
};

inline void Face::SetVertexIndex(int facePointIndex, int vertexIndex)
{
	mVerticeIndex[facePointIndex] = vertexIndex;
}

inline void Face::SetNormal(int facePointIndex, const vec3f& normal)
{
	mNormals[facePointIndex] = normal;
}

inline void Face::SetST(int facePointIndex, const vec2f& st)
{
	mSTs[facePointIndex] = st;
}

inline unsigned int Face::GetVertexIndex(int facePointIndex)
{
	return mVerticeIndex[facePointIndex];
}

inline vec3f& Face::GetNormal(int facePointIndex)
{
	return mNormals[facePointIndex];
}

inline vec2f& Face::GetST(int facePointIndex)
{
	return mSTs[facePointIndex];
}
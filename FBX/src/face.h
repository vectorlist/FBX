#pragma once

#include <config.h>
#include <vec2f.h>
#include <vec3f.h>

class Face
{
public:
	void			setVertexIndex(int facePointIndex, int vertexIndex);
	void			setNormal(int facePointIndex, const vec3f& normal);
	void			setST(int facePointIndex, const vec2f& st);

	unsigned int	getVertexIndex(int facePointIndex);
	vec3f&			getNormal(int facePointIndex);
	vec2f&			getST(int facePointIndex);

private:
	unsigned int	mVerticeIndex[FACE_COMPONENT_NUM];
	vec3f			mNormals[FACE_COMPONENT_NUM];
	vec2f			mSTs[FACE_COMPONENT_NUM];
};

inline void Face::setVertexIndex(int facePointIndex, int vertexIndex)
{
	mVerticeIndex[facePointIndex] = vertexIndex;
}

inline void Face::setNormal(int facePointIndex, const vec3f& normal)
{
	mNormals[facePointIndex] = normal;
}

inline void Face::setST(int facePointIndex, const vec2f& st)
{
	mSTs[facePointIndex] = st;
}

inline unsigned int Face::getVertexIndex(int facePointIndex)
{
	return mVerticeIndex[facePointIndex];
}

inline vec3f& Face::getNormal(int facePointIndex)
{
	return mNormals[facePointIndex];
}

inline vec2f& Face::getST(int facePointIndex)
{
	return mSTs[facePointIndex];
}
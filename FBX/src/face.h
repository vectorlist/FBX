#pragma once

#include <config.h>
#include <vec2f.h>
#include <vec3f.h>

class Face
{
public:
	void setVertexIndex(int facePointID, int vertexID) {
		mVerticeIndex[facePointID] = vertexID;
	}

	unsigned int getVertexIndex(int facePointID) {
		return mVerticeIndex[facePointID];
	}

	void setNormal(int facePointID, const vec3f& normal)
	{
		mNormals[facePointID] = normal;
	}
	vec3f& getNormal(int facePointID)
	{
		return mNormals[facePointID];
	}

	void setST(int facePointID, const vec2f& st)
	{
		mSTs[facePointID] = st;
	}
	vec2f& getST(int facePointID)
	{
		return mSTs[facePointID];
	}

private:
	unsigned int mVerticeIndex[FACE_COMPONENT_NUM];
	vec3f mNormals[FACE_COMPONENT_NUM];
	vec2f mSTs[FACE_COMPONENT_NUM];
};
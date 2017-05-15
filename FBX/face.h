#pragma once

#define FACE_ELEMENT_MAX		3

class Face
{
public:
	void setVertexIndex(int cornerId, int vertexID) {
		m_verticesIndex[cornerId] = vertexID;
	}

	unsigned int getVertexIndex(int cornerID) {
		return m_verticesIndex[cornerID];
	}
	void setNormal(int elementIndex, const vec3f& normal)
	{
		m_normals[elementIndex] = normal;
	}
	vec3f& getNormal(int elementIndex)
	{
		return m_normals[elementIndex];
	}

	void setST(int elementIndex, const vec2f& st)
	{
		m_sts[elementIndex] = st;
	}
	vec2f& getST(int elementIndex)
	{
		return m_sts[elementIndex];
	}

private:
	unsigned int m_verticesIndex[FACE_ELEMENT_MAX];
	vec3f m_normals[FACE_ELEMENT_MAX];
	vec2f m_sts[FACE_ELEMENT_MAX];
};
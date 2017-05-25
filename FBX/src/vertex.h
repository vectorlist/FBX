#pragma once

#include <fbxsdk/core/math/fbxvector4.h>
#include <vec3f.h>
#include <config.h>

class Vertex
{
public:
	Vertex();
	~Vertex();

	bool addWeight(const unsigned int boneID, const float weight);

	float getBoneWeight(uint32_t index);
	float* getBoneWeights();
	uint32_t getBoneID(uint32_t index);
	uint32_t* getBoneIDs();

	void setPosition(const fbxsdk::FbxVector4 &pos);
	vec3f& getPosition();
private:
	vec3f m_posotion;
	unsigned int m_boneIDs[BONE_COMPONENT_NUM];
	float m_boneWeights[BONE_COMPONENT_NUM];
	unsigned int mBoneComponentIndex;
};

inline float Vertex::getBoneWeight(uint32_t index)
{
	return m_boneWeights[index];
}

inline float* Vertex::getBoneWeights() 
{
	return m_boneWeights;
}

inline uint32_t Vertex::getBoneID(uint32_t index)
{
	return m_boneIDs[index];
}

inline uint32_t* Vertex::getBoneIDs()
{
	return m_boneIDs;
}

inline void Vertex::setPosition(const fbxsdk::FbxVector4 &pos)
{
	m_posotion.x = (float)pos[0];
	m_posotion.y = (float)pos[1];
	m_posotion.z = (float)pos[2];
}

inline vec3f& Vertex::getPosition()
{
	// TODO: insert return statement here
	return m_posotion;
}

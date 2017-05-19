#include <vertex.h>
#include <log.h>

Vertex::Vertex()
	: mBoneComponentIndex(0)
{
	for (int i = 0; i < BONE_COMPONENT_NUM; ++i)
	{
		m_boneIDs[i] = 0;
		m_boneWeights[i] = 0.0f;
	}
}


Vertex::~Vertex()
{
}

bool Vertex::addWeight(const unsigned int boneID, const float weight)
{
	if(mBoneComponentIndex > BONE_COMPONENT_NUM)
	{
		LOG << "out of num bone properties index" << ENDN;
		return false;
	}
	m_boneIDs[mBoneComponentIndex] = boneID;
	m_boneWeights[mBoneComponentIndex] = weight;
	++mBoneComponentIndex;
	return true;
}

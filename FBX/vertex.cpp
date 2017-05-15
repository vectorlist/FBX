#include <vertex.h>
#include <log.h>

Vertex::Vertex()
	: m_bonePropertiesIndex(0)
{
	for (int i = 0; i < NUM_BONE_PROPERTIES; ++i)
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
	if(m_bonePropertiesIndex > NUM_BONE_PROPERTIES)
	{
		LOG << "out of num bone properties index" << ENDN;
		return false;
	}
	m_boneIDs[m_bonePropertiesIndex] = boneID;
	m_boneWeights[m_bonePropertiesIndex] = weight;
	++m_bonePropertiesIndex;
	return true;
}

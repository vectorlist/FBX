#include "Vertex.h"
#include "BoneNode.h"
#include <log.h>


Vertex::Vertex()
	: m_numInfluences(0)
{
	for(int influence = 0; influence < MAX_INFLUENCES; influence++)
	{
		m_boneInfluenceIds[influence] = 0; //Max 4 joints per vertex (could use uint8)
		m_boneWeights[influence] = 0.0f;
	}

}

bool Vertex::AddWeight(
	const unsigned int boneInfluenceid,
	const float weight
)
{
	if (m_numInfluences > MAX_INFLUENCES)
	{
		LOG_ERROR("This mesh contains more than %d bone influences, which is not currently supported. Aborting load.", MAX_INFLUENCES);
		return false;
	}

	m_boneInfluenceIds[m_numInfluences] = boneInfluenceid;
	m_boneWeights[m_numInfluences] = weight;
	m_numInfluences++;

	return true;
}

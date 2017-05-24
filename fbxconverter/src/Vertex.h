#pragma once

#include "MeshFwdDecl.h"


class Vertex
{
public:
	Vertex::Vertex();

	bool AddWeight(
		const unsigned int boneInfluenceid,
		const float weight
	);

	inline float GetBoneWeight(
		int influenceIndex
	)
	{
		return m_boneWeights[influenceIndex];
	}

	inline float* GetBoneWeights()
	{
		return m_boneWeights;
	}

	inline unsigned int GetBoneInfluence(
		int influenceIndex
		)
	{
		return m_boneInfluenceIds[influenceIndex];
	}

	inline unsigned int *GetBoneInfluences()
	{
		return m_boneInfluenceIds;
	}

	inline unsigned int GetNumInfluences()
	{
		return m_numInfluences;
	}

	inline unsigned int GetBoneInfluenceId(
		int influenceIndex
		)
	{
		return m_boneInfluenceIds[influenceIndex];
	}

	inline void SetPosition(
		FbxVector4 position
	)
	{
		m_position = position;
	}

	inline FbxVector4 GetPosition()
	{
		return m_position;
	}

private:
	
	FbxVector4 m_position;
	unsigned int m_boneInfluenceIds[MAX_INFLUENCES]; //MAX_INFLUENCES joints per vertex (could use uint8)
	float m_boneWeights[MAX_INFLUENCES];
	unsigned int m_numInfluences;
};


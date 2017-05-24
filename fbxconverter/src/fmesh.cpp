#include "fmesh.h"
#include "BoneNode.h"
#include "MeshNode.h"


Mesh::Mesh()
{
}

BoneNode* Mesh::GetBoneByName(
	const std::string &name
)
{
	for(BoneNode *node = m_boneNodes.m_root; node != NULL; node = m_boneNodes.GetNextChildFirst(node))
	{
		if(node->GetName() == name)
		{
			return node;
		}
	}

	return NULL;
}

Mesh::~Mesh()
{
	// Reset the node id's
	BoneNode::ResetIdCounter();
	MeshNode::ResetIdCounter();
}


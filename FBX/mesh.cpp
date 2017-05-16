#include "mesh.h"


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	BoneNode::resetCounter();
}

BoneNode* Mesh::getBoneNodeRoot()
{
	return m_boneNodes.m_root;
}

BoneNode* Mesh::getBoneNodeByName(const std::string name)
{
	for (BoneNode* node = m_boneNodes.m_root; node != NULL; node = m_boneNodes.GetNextChildFirst(node))
	{
		if (node->getName() == name) {
			return node;
		}
	}
	return NULL;
}

void Mesh::addChildNodeBone(BoneNode *parent, BoneNode *node)
{
	m_boneNodes.AddAsChild(parent, node);
	m_boneNodesNum++;
}

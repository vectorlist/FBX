#pragma once

#include <bonenode.h>

class Mesh
{
public:
	Mesh();
	~Mesh();

	BoneNode* getBoneNodeRoot();
	void addChildNodeBone(BoneNode* parent, BoneNode *node);
	BoneNode* getBoneNodeByName(const std::string name);

	LinkedTree<BoneNode> m_boneNodes;
	uint32_t m_boneNodesNum;
};


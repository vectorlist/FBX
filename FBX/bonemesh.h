#pragma once

#include <bonenode.h>

class FBXBoneMesh
{
public:
	FBXBoneMesh();
	~FBXBoneMesh();
	void addChildBoneNode(BoneNode* parent, BoneNode* node);

	BoneNode* getBoneNodeByName(const std::string &name);

	NodeTree<BoneNode> mBoneNodes;
	//node bone ID, weight;
	uint32_t mBoneNodesNum;
};
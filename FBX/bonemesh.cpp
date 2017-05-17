#include <bonemesh.h>

FBXBoneMesh::FBXBoneMesh()
	: mBoneNodesNum(0)
{

}

FBXBoneMesh::~FBXBoneMesh()
{

}

void FBXBoneMesh::addChildBoneNode(BoneNode* parent, BoneNode* node)
{
	mBoneNodes.addChild(parent, node);
	//increase bone num when add 
	mBoneNodesNum++;
}

BoneNode* FBXBoneMesh::getBoneNodeByName(const std::string &name)
{
	for (	BoneNode* node = mBoneNodes.getRoot();
			node != NULL;
			node = mBoneNodes.getNextChildFirst(node))
	{
		if (node->getName() == name) {
			return node;
		}
	}
	//empty
	return NULL;
}

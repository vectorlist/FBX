#pragma once

#include <bonenode.h>
#include <meshnode.h>
#include <animation.h>
#include <track.h>

class Node
{
public:
	Node();
	~Node();

	/*------------ BONE NODE -------------*/
	BoneNode* getBoneNodeRoot();
	void addChildBoneNode(BoneNode* parent, BoneNode* node);
	BoneNode* getBoneNodeByName(const std::string &name);

	/*------------ MESH NODE -------------*/
	MeshNode* getMeshNodeRoot();
	void addChildMeshNode(MeshNode* parent, MeshNode* node);
	//dont care get by name
	MeshNode* getCurrentMeshNode();
	

	NodeTree<BoneNode> mBoneNodes;
	NodeTree<MeshNode> mMeshNodes;
	//node bone ID, weight;
	uint32_t mBoneNodesNum;
	uint32_t mMeshNodesNum;

	/*------------ ANIMATION -------------*/
	void setAnimation(AnimationPtr &animation);
	Animation* getAnimation() const;

private:
	AnimationPtr mAnimationPtr;
};
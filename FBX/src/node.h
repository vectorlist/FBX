#pragma once

#include <bonenode.h>
#include <meshnode.h>
#include <animationscene.h>
#include <track.h>
#include <config.h>

class Node
{
public:
	Node();
	~Node();

	/*------------ BONE NODE -------------*/
	BoneNode* getBoneNodeRoot();
	BoneNode* getBoneNodeByName(const std::string &name);
	void addChildBoneNode(BoneNode* parent, BoneNode* node);

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
	void setAnimationSample(AnimationSamplePtr &animation);
	AnimationSample* getAnimationSample() const;

	void setAnimationLayers(AnimationLayersPtr layers);
	AnimationLayers* getAnimationLayers() const;
	AnimationSample* currentSample = NULL;
private:
	AnimationSamplePtr mAnimationSamplePtr;
	AnimationLayersPtr mLayers;
};
#pragma once

#include <bonenode.h>
#include <meshnode.h>
#include <animhandle.h>
#include <track.h>
#include <config.h>

class FlimBoxDevice;
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
	MeshNode* getCurrentMeshNode();

	/*------------ ANIMATION -------------*/
	void setAnimationLayerPtr(animlayer_ptr layerPtr);
	AnimLayer* getAnimationLayer() const;

	void setCurrentSample(AnimSample* sample);
	AnimSample* getCurrentSample() const;

	bool hasAnimation();
private:
	NodeTree<BoneNode> mBoneNodes;
	NodeTree<MeshNode> mMeshNodes;

	animlayer_ptr mAnimationLayerPtr;
	AnimSample* mCurrentSample;
};
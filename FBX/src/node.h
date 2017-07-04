#pragma once

#include <bonenode.h>
#include <meshnode.h>
#include <animhandle.h>
#include <track.h>
#include <config.h>
#include <string>

class AnimNode;
class FlimBoxNode;
class Node
{
public:
	Node();
	~Node();

	/*------------ BONE NODE -------------*/
	BoneNode*			GetBoneNodeRoot();
	BoneNode*			GetBoneNodeByName(const std::string &name);
	void				AddChildBoneNode(BoneNode* parent, BoneNode* node);

	/*------------ MESH NODE -------------*/
	MeshNode*			GetMeshNodeRoot();
	void				AddChildMeshNode(MeshNode* parent, MeshNode* node);
	MeshNode*			GetCurrentMeshNode();

	/*------------ ANIMATION -------------*/
	void				SetAnimationLayerPtr(animlayer_ptr layerPtr);
	AnimLayer*			GetAnimationLayer() const;

	void				SetCurrentSample(AnimSample* sample);
	AnimSample*			GetCurrentSample() const;
	void				SetNextSample(AnimSample* sample);
	AnimSample*			GetNextSample() const;
	AnimSample*			GetBaseSample();

	bool				HasAnimation();
private:
	NodeTree<BoneNode>	mBoneNodes;
	NodeTree<MeshNode>	mMeshNodes;

	animlayer_ptr		mAnimationLayerPtr;
	AnimSample*			mCurrentSample;
	AnimSample*			mNextSample;
};
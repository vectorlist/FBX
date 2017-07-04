#include <node.h>
#include <animlayer.h>

Node::Node()
	: mCurrentSample(NULL), mNextSample(NULL)
{

}

Node::~Node()
{
	
}

/*------------ BONE NODE -------------*/

BoneNode *Node::GetBoneNodeRoot()
{
	return mBoneNodes.mRoot;
}

void Node::AddChildBoneNode(BoneNode* parent, BoneNode* node)
{
	mBoneNodes.AddChild(parent, node);
}

BoneNode* Node::GetBoneNodeByName(const std::string &name)
{
	for (BoneNode* node = mBoneNodes.mRoot;
		node != NULL;
		node = mBoneNodes.GetNextChildFirst(node))
	{
		if (node->GetName() == name) {
			return node;
		}
	}
	//empty
	return NULL;
}

/*------------ MESH NODE -------------*/

MeshNode* Node::GetMeshNodeRoot()
{
	return mMeshNodes.mRoot;
}

void Node::AddChildMeshNode(MeshNode* parent, MeshNode *node)
{
	mMeshNodes.AddChild(parent, node);
}

MeshNode* Node::GetCurrentMeshNode()
{
	for (auto* node = mMeshNodes.mRoot;
		node != NULL;
		node = mMeshNodes.GetNextChildFirst(node))
	{
		if (node) {
			return node;
		}
	}
	return NULL;
}

void Node::SetAnimationLayerPtr(animlayer_ptr layerPtr)
{
	mAnimationLayerPtr = layerPtr;
}

AnimLayer* Node::GetAnimationLayer() const
{
	return mAnimationLayerPtr.get();
}

void Node::SetCurrentSample(AnimSample *sample)
{
	mCurrentSample = sample;
}

AnimSample* Node::GetCurrentSample() const
{
	return mCurrentSample;
}

void Node::SetNextSample(AnimSample *sample)
{
	mNextSample = sample;
}

AnimSample* Node::GetNextSample() const
{
	return mNextSample;
}

AnimSample* Node::GetBaseSample()
{
	//Base Sample
	return mAnimationLayerPtr->getSample(0);
}

bool Node::HasAnimation()
{
	return mBoneNodes.Size() > 0;
}


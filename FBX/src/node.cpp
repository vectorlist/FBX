#include <node.h>
#include <animlayer.h>

Node::Node()
	: mCurrentSample(NULL)
{

}

Node::~Node()
{
	
}

/*------------ BONE NODE -------------*/

BoneNode *Node::getBoneNodeRoot()
{
	return mBoneNodes.mRoot;
}

void Node::addChildBoneNode(BoneNode* parent, BoneNode* node)
{
	mBoneNodes.addChild(parent, node);
}

BoneNode* Node::getBoneNodeByName(const std::string &name)
{
	for (BoneNode* node = mBoneNodes.mRoot;
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

/*------------ MESH NODE -------------*/

MeshNode* Node::getMeshNodeRoot()
{
	return mMeshNodes.mRoot;
}

void Node::addChildMeshNode(MeshNode* parent, MeshNode *node)
{
	mMeshNodes.addChild(parent, node);
}

MeshNode* Node::getCurrentMeshNode()
{
	for (auto* node = mMeshNodes.mRoot;
		node != NULL;
		node = mMeshNodes.getNextChildFirst(node))
	{
		if (node) {
			return node;
		}
	}
	return NULL;
}

void Node::setAnimationLayerPtr(animlayer_ptr layerPtr)
{
	mAnimationLayerPtr = layerPtr;
}

AnimLayer* Node::getAnimationLayer() const
{
	return mAnimationLayerPtr.get();
}

void Node::setCurrentSample(AnimSample *sample)
{
	mCurrentSample = sample;
}

AnimSample * Node::getCurrentSample() const
{
	return mCurrentSample;
}

bool Node::hasAnimation()
{
	return mBoneNodes.size() > 1;
}



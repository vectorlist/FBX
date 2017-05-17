#include <node.h>

Node::Node()
	: mBoneNodesNum(0), mMeshNodesNum(0)
{

}

Node::~Node()
{

}

/*------------ BONE NODE -------------*/

BoneNode *Node::getBoneNodeRoot()
{
	return mBoneNodes.getRoot();
}

void Node::addChildBoneNode(BoneNode* parent, BoneNode* node)
{
	mBoneNodes.addChild(parent, node);
	//increase bone num when add 
	mBoneNodesNum++;
}

BoneNode* Node::getBoneNodeByName(const std::string &name)
{
	for (BoneNode* node = mBoneNodes.getRoot();
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

MeshNode * Node::getMeshNodeRoot()
{
	return mMeshNodes.getRoot();
}

void Node::addChildMeshNode(MeshNode* parent, MeshNode *node)
{
	mMeshNodes.addChild(parent, node);
	mMeshNodesNum++;
}

MeshNode* Node::getCurrentMeshNode()
{
	for (auto* node = mMeshNodes.getRoot(); node != NULL;)
	{
		if (node) {
			if (!node->isBuilt) continue;
			return node;
		}
		node = mMeshNodes.getNextChildFirst(node);
	}
	return NULL;
}

void Node::setAnimation(AnimationPtr & animation)
{
	mAnimationPtr = std::move(animation);
}

Animation* Node::getAnimation() const
{
	if (mAnimationPtr) {
		return mAnimationPtr.get();
	}
	return NULL;
}




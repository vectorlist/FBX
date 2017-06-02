#include "meshnode.h"

MeshNode::MeshNode()
{
}

MeshNode::~MeshNode()
{
	
}

void MeshNode::setName(const std::string &name)
{
	mName = name;
}

const std::string & MeshNode::getName() const
{
	return mName;
}


FaceArray& MeshNode::getFaces()
{
	return mFaces;
}

PointArray& MeshNode::getPoints()
{
	return mPoints;
}

const FbxAMatrix& MeshNode::getGlobalTransform() const
{
	return m_globalTransform;
}

const FbxAMatrix& MeshNode::getLocalTransform() const
{
	return m_localTransform;
}

void MeshNode::setGlobalTransform(const FbxAMatrix &transform)
{
	m_globalTransform = transform;
}

void MeshNode::setLocalTransform(const FbxAMatrix &transform)
{
	m_localTransform = transform;
}

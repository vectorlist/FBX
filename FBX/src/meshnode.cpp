#include "meshnode.h"

MeshNode::MeshNode()
{
}

MeshNode::~MeshNode()
{
	
}

void MeshNode::SetName(const std::string &name)
{
	mName = name;
}

const std::string& MeshNode::GetName() const
{
	return mName;
}

std::vector<Face>& MeshNode::GetFaces()
{
	return mFaces;
}

std::vector<Point>& MeshNode::GetPoints()
{
	return mPoints;
}

const Matrix4x4& MeshNode::GetGlobalTransform() const
{
	return mGlobal;
}

const Matrix4x4& MeshNode::GetLocalTransform() const
{
	return mLocal;
}

void MeshNode::SetGlobalTransform(const Matrix4x4 &transform)
{
	mGlobal = transform;
}

void MeshNode::SetLocalTransform(const Matrix4x4 &transform)
{
	mLocal = transform;
}

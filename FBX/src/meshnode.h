#pragma once

#include <nodetree.h>
#include <face.h>
#include <point.h>
#include <vector>
#include <matrix4x4.h>

class MeshNode : public NodeTreeItem<MeshNode>
{
public:
	MeshNode();
	~MeshNode();

	void					SetName(const std::string &name);
	const std::string&		GetName() const;

	std::vector<Face>&		GetFaces();
	std::vector<Point>&		GetPoints();

	const Matrix4x4&		GetGlobalTransform() const;
	const Matrix4x4&		GetLocalTransform() const;
	void					SetGlobalTransform(const Matrix4x4 &transform);
	void					SetLocalTransform(const Matrix4x4 &transform);

private:
	Matrix4x4				mGlobal;
	Matrix4x4				mLocal;

	std::string				mName;
	std::vector<Face>		mFaces;
	std::vector<Point>		mPoints;

};

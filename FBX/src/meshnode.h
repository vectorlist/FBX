#pragma once

#include <nodetree.h>
#include <face.h>
#include <point.h>
#include <vector>
#include <fbxsdk/core/math/fbxaffinematrix.h>

using namespace fbxsdk;
typedef std::vector<Face> FaceArray;
typedef std::vector<Point> PointArray;

class MeshNode : public NodeTreeItem<MeshNode>
{
public:
	MeshNode();
	~MeshNode();

	void setName(const std::string &name);
	const std::string& getName() const;

	FaceArray& getFaces();
	PointArray& getPoints();

	const FbxAMatrix &getGlobalTransform() const;
	const FbxAMatrix &getLocalTransform() const;
	void setGlobalTransform(const FbxAMatrix &transform);
	void setLocalTransform(const FbxAMatrix &transform);

private:
	FbxAMatrix m_localTransform;
	FbxAMatrix m_globalTransform;

	std::string mName;
	FaceArray mFaces;
	PointArray mPoints;

};

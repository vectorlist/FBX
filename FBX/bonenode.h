#pragma once

#include <nodetree.h>
#include <string>
#include <fbxsdk/core/math/fbxaffinematrix.h>

using namespace fbxsdk;

class BoneNode : public NodeTreeItem<BoneNode>
{
public:
	BoneNode();
	~BoneNode();

	void setName(const std::string &name);
	const std::string& getName() const;
	uint32_t getID();
	static void resetGlobalID();
private:
	//TODO replace to map(int32, string) as bone name and id
	std::string mName;
	uint32_t mID;

	static uint32_t globalID;

	//transform matrix
public:
	FbxAMatrix mGlobalTransform;
	FbxAMatrix mInverseTransform;
};


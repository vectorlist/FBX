#pragma once

#include <linkedtree.h>
#include <string>
#include <fbxsdk/core/math/fbxaffinematrix.h>

using namespace fbxsdk;

class BoneNode : public LinkedTreeItem<BoneNode>
{
public:
	BoneNode();
	~BoneNode();

	void setName(const std::string &name);
	const std::string& getName();
	unsigned int getID() const;

	static void resetCounter();

private:
	unsigned int m_id;
	std::string m_name;

	static unsigned int m_counter;
	
	FbxAMatrix m_globalTransform;
	FbxAMatrix m_inverseRefferenceMatrix;
};


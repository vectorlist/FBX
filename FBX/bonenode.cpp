#include "BoneNode.h"

uint32_t BoneNode::globalID = 0;

BoneNode::BoneNode()
	: mID(globalID++)
{

}

BoneNode::~BoneNode()
{

}

void BoneNode::setName(const std::string & name)
{
	mName = name;
}

const std::string& BoneNode::getName() const
{
	return mName;
}

uint32_t BoneNode::getID()
{
	return mID;
}

void BoneNode::resetGlobalID()
{
	globalID = 0;
}

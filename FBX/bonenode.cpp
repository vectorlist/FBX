#include "bonenode.h"

unsigned int BoneNode::m_counter = 0;

BoneNode::BoneNode()
	: m_id(m_counter++)
{

}


BoneNode::~BoneNode()
{
}

void BoneNode::setName(const std::string &name)
{
	m_name = name;
}

const std::string& BoneNode::getName()
{
	return m_name;
	// TODO: insert return statement here
}

unsigned int BoneNode::getID() const
{
	return m_id;
}

void BoneNode::resetCounter()
{
	m_counter = 0;
}

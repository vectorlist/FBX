#include "bonenode.h"

unsigned int BoneNode::m_counter = 0;

BoneNode::BoneNode()
	: m_id(++m_counter)
{

}


BoneNode::~BoneNode()
{
}

void BoneNode::setName(const std::string &name)
{
	m_name = name;
}

#pragma once

#include <linkedtree.h>
#include <string>

class BoneNode : public LinkedTreeItem<BoneNode>
{
public:
	BoneNode();
	~BoneNode();

	void setName(const std::string &name);

	unsigned int m_id;
	static unsigned int m_counter;
	std::string m_name;
};


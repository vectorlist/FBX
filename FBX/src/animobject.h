#pragma once

#include <string>
#include <fbxcore.h>
#include <fbxdevice.h>

class AnimObject
{
public:
	AnimObject(const std::string &filename);
	virtual~AnimObject(){}


	Node* node;
};

inline AnimObject::AnimObject(const std::string &filename)
{

	FBXCore core(filename);


}
#pragma once

#include "Key.h"

class VectorKey : public Key
{
public :

	VectorKey() 
		: Key(0)
	{
		m_vector[0] = 0;
		m_vector[1] = 0;
		m_vector[2] = 0;
	}

	VectorKey(
		double x, 
		double y,
		double z,
		long time) 
		: Key(time)
	{
		m_vector[0] = x;
		m_vector[1] = y;
		m_vector[2] = z;
	}

	VectorKey(
		FbxVector4 vector, 
		long time) 
		: Key(time)
	{
		m_vector = vector;
	}

	FbxVector4 m_vector;
};


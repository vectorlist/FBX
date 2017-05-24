#pragma once

#include "Key.h"

class QuaternionKey : public Key
{
public :

	QuaternionKey() 
		: Key(0)
	{
		m_quaternion[0] = 0;
		m_quaternion[1] = 0;
		m_quaternion[2] = 0;
		m_quaternion[3] = 0;
	}

	QuaternionKey(
		double x, 
		double y,
		double z,
		double w,
		long time) 
		: Key(time)
	{
		m_quaternion[0] = x;
		m_quaternion[1] = y;
		m_quaternion[2] = z;
		m_quaternion[3] = w;
	}

	QuaternionKey(
		FbxQuaternion quaternion, 
		long time
		) : Key(time)
	{
		m_quaternion = quaternion;
	}

	FbxQuaternion m_quaternion;

};


#pragma once

#include <cstdlib>
#include <fbxsdk/core/math/fbxvector4.h>
#include <fbxsdk/core/math/fbxquaternion.h>
#include <vector>
#include <memory>

using namespace fbxsdk;

class Key
{
public:
	Key(long time) : mTime(time){}
	virtual~Key() {};
	
	long mTime;
};

class KeyVector : public Key
{
public:
	KeyVector() : Key(0)
	{
		mVector[0] = 0;
		mVector[1] = 0;
		mVector[2] = 0;
	}

	KeyVector(double x, double y, double z, long time)
		: Key(time)
	{
		mVector[0] = x;
		mVector[1] = y;
		mVector[2] = z;
	}

	KeyVector(const FbxVector4 &other, long time)
		: Key(time)
	{
		mVector = other;
	}

	FbxVector4 mVector;

	friend std::ostream& operator<<(std::ostream &o, const KeyVector &v)
	{
		o << "keyvector( " 
			<< v.mVector[0] << ", " 
			<< v.mVector[1] << " ," 
			<< v.mVector[2] << ')';
		return o;
	}
};


class KeyQuaternion : public Key
{
public:
	KeyQuaternion()
		: Key(0)
	{
		mQuaternion[0] = 0;
		mQuaternion[1] = 0;
		mQuaternion[2] = 0;
		mQuaternion[3] = 0;
	}

	KeyQuaternion(double x, double  y, double z, double w, long time)
		: Key(time)
	{
		mQuaternion[0] = x;
		mQuaternion[1] = y;
		mQuaternion[2] = z;
		mQuaternion[3] = w;
	}

	KeyQuaternion(const FbxQuaternion &quaternion, long time)
		: Key(time)
	{
		mQuaternion = quaternion;
	}

	friend std::ostream& operator<<(std::ostream &o, const KeyQuaternion &q)
	{
		o << "keyQuaternion( "
			<< q.mQuaternion[0] << ", "
			<< q.mQuaternion[1] << " ,"
			<< q.mQuaternion[2] << " ,"
			<< q.mQuaternion[3] << ')';
		return o;
	}

private:
	FbxQuaternion mQuaternion;
};

typedef std::vector<KeyVector> KeyVectorArray;
typedef std::vector<KeyQuaternion> KeyQuaternionArray;

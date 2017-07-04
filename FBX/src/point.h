#pragma once

#include <fbxsdk/core/math/fbxvector4.h>
#include <vec3f.h>
#include <config.h>

class Point
{
public:
	Point();
	~Point() {};

	float			GetBoneWeight(unsigned int index);
	float*			GetBoneWeights();
	unsigned int	getBoneID(unsigned int index);
	unsigned int*	GetBoneIDs();
	const vec3f&	GetPosition() const;

	void			SetPosition(const fbxsdk::FbxVector4 &pos);
	bool			AddWeight(const unsigned int boneID, const float weight);
private:
	vec3f			mPosition;
	unsigned int	mBoneIDs[BONE_COMPONENT_NUM];
	float			mBoneWeights[BONE_COMPONENT_NUM];
	unsigned int	mBoneComponentIndex;
};

inline Point::Point()
	: mBoneComponentIndex(0)
{
	memset(mBoneIDs, 0, sizeof(mBoneIDs));
	memset(mBoneWeights, 0.0f, sizeof(mBoneWeights));
}

inline bool Point::AddWeight(const unsigned int boneID, const float weight)
{
	if (mBoneComponentIndex > BONE_COMPONENT_NUM)
	{
		return false;
	}
	mBoneIDs[mBoneComponentIndex] = boneID;
	mBoneWeights[mBoneComponentIndex] = weight;
	++mBoneComponentIndex;
	return true;
}


inline float Point::GetBoneWeight(unsigned int index)
{
	return mBoneWeights[index];
}

inline float* Point::GetBoneWeights() 
{
	return mBoneWeights;
}

inline unsigned int Point::getBoneID(unsigned int index)
{
	return mBoneIDs[index];
}

inline unsigned int* Point::GetBoneIDs()
{
	return mBoneIDs;
}

inline void Point::SetPosition(const fbxsdk::FbxVector4 &pos)
{
	mPosition.x = (float)pos[0];
	mPosition.y = (float)pos[1];
	mPosition.z = (float)pos[2];
}

inline const vec3f& Point::GetPosition() const
{
	return mPosition;
}

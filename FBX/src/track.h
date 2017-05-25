#pragma once

#include <key.h>

class TrackVec3
{
public:
	TrackVec3(const int totalKeyNum)
	{
		//for acllocate memory this is size 0
		mKeys.reserve(totalKeyNum);
	}

	void addKey(const KeyVec3 &key)
	{
		mKeys.push_back(key);
	}

	KeyVec3& getKey(int keyIndex)
	{
		return mKeys[keyIndex];
	}
	int size() const { return mKeys.size(); }
private:
	KeyVectorArray mKeys;
};

class TrackQuaternion
{
public:
	TrackQuaternion(const int totalKeyNum)
	{
		mKeys.reserve(totalKeyNum);
	}

	void addKey(const KeyQuaternion& key)
	{
		mKeys.push_back(key);
	}

	KeyQuaternion& getKey(int keyIndex)
	{
		return mKeys[keyIndex];
	}
	KeyQuaternionArray mKeys;
};

typedef std::shared_ptr<TrackVec3> TrackVec3Ptr;
typedef std::shared_ptr<TrackQuaternion> TrackQuaternionPtr;
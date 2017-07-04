#pragma once

#include <animkey.h>

#define TRACK_INTERVAL		1

class VectorTrack
{
public:
	VectorTrack(const int totalKeyNum) : mKeyNums(totalKeyNum)
	{
		//for acllocate memory this is size 0
		mKeys.reserve(totalKeyNum);
	}

	void AddKey(const VectorKey &key)
	{
		mKeys.push_back(key);
	}

	VectorKey& GetKey(int keyIndex)
	{
		return mKeys[keyIndex];
	}
	int mKeyNums;
private:
	std::vector<VectorKey> mKeys;
};

class QautTrack
{
public:
	QautTrack(const int totalKeyNum) : mKeyNums(totalKeyNum)
	{
		mKeys.reserve(totalKeyNum);
	}

	void AddKey(const QuatKey& key)
	{
		mKeys.push_back(key);
	}

	QuatKey& GetKey(int keyIndex)
	{
		return mKeys[keyIndex];
	}
	int mKeyNums;
	std::vector<QuatKey> mKeys;
};

typedef std::shared_ptr<VectorTrack> VectorTrackPtr;
typedef std::shared_ptr<QautTrack> QautTrackPtr;
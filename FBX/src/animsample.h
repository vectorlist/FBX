#pragma once

#include <string>
#include <log.h>
#include <memory>

#define GET_TIME()						timeGetTime()

class AnimSample 
{
public:
	AnimSample();

	float mStart;
	float mEnd;
	float mDuration;

	int mFrameStart;
	int mFrameEnd;
	int mFrameCount;
	float mFps;

	int mIndex;
	std::string mName;
	
};

inline AnimSample::AnimSample()
	:
	mStart(0.0f),
	mEnd(0.0f),
	mDuration(0.0f),
	mFrameStart(0),
	mFrameEnd(0),
	mFrameCount(0),
	mIndex(0)
{

}


class anim_sample_ptr;
typedef std::shared_ptr<AnimSample> animsample_ptr;
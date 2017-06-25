#pragma once

#include <string>
#include <log.h>
#include <memory>

#define GET_TIME()						timeGetTime()

class AnimSample 
{
public:
	AnimSample();
	~AnimSample();

	float mStart;
	float mEnd;
	float mDuration;

	int mFrameStart;
	int mFrameEnd;
	int mFrameCount;
	float mFps;

	std::string mName;
	
};

class anim_sample_ptr;
typedef std::shared_ptr<AnimSample> animsample_ptr;
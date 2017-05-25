#pragma once

#include <string>
#include <log.h>
#include <memory>

#define GET_TIME()						timeGetTime()
#define ANIMATION_FRAME_MIN				0.01f
#define ANIMATION_FRAME_MAX				1000.f
#define MILLISECOND						1000.f
#define MILLIHALFSECOND					500.f
#define ANIMATION_CURRENT_FRAME_RATE	30.f

class AnimationSample 
{
public:
	AnimationSample();
	~AnimationSample();

	void setName(const std::string &name);
	void setFrameNums(int frameNum);
	void setFps(float fps);
	void setSampleStart(int startSample);
	void setSampleEnd(int endSample);

	const std::string& getName() const;
	float getFps() const;
	int getSamplesFrameNum() const;
	int getSampleStart() const;
	int getSampleEnd() const;
	int getSampleOffset() const;

	long convertFrameToMilli(const int frame);
	int convertMilliToFrame(const long milli);

private:
	std::string mName;
	int mFramesNum;
	int mSampleStart;
	int mSampleEnd;

	float mFps;
	float mMilliSecondFrame;
	float mMilliSecondPerHalfFrame;
	
};

typedef std::shared_ptr<AnimationSample> AnimationSamplePtr;
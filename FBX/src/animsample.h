#pragma once

#include <string>
#include <log.h>
#include <memory>

#define GET_TIME()						timeGetTime()
#define ANIMATION_FRAME_MIN				0.01f
#define ANIMATION_FRAME_MAX				1000.f
#define MILLISECOND						1000.f
#define MILLIHALFSECOND					500.f
#define ANIMATION_DEFAULT_FRAME_RATE	30.f

class AnimSample 
{
public:
	AnimSample();
	~AnimSample();

	void setName(const std::string &name);
	void setFrameNums(int frameNum);
	void setFps(float fps);
	void setFrameStart(int startFrame);
	void setFrameEnd(int endFrame);
	void setSampleStart(int startSample);
	void setSampleEnd(int endSample);

	const std::string& getName() const;
	float getFps() const;
	int getSamplesFrameNum() const;
	int getFrameStart() const;
	int getFrameEnd() const;
	int getSampleStart() const;
	int getSampleEnd() const;
	int getSampleBlock() const;

	long convertFrameToMilli(const int frame);
	int convertMilliToFrame(const long milli);

private:
	std::string mName;
	int mFramesNum;
	int mSampleStart;
	int mSampleEnd;

	int mFrameStart;
	int mFrameEnd;

	float mFps;
	float mMilliSecondFrame;
	float mMilliSecondPerHalfFrame;
	
};

class anim_sample_ptr;
typedef std::shared_ptr<AnimSample> animsample_ptr;
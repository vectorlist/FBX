#pragma once

#include <memory>

#define TIME_MILLISECOND		1000.0f
#define TIME_HALFMILLISECOND	500.0f

class AnimationSample
{
public:
	AnimationSample();
	~AnimationSample();

	void setFramePerSeconds(float fps);
	void setFrameNum(int numFrames);
	void setSampleStart(int sampleStart);
	void setSampleEnd(int sampleEnd);

	double getFps() const;
	int getFrameNum() const;
	int getFrameSampleNum() const;
	int getSampleStart() const;
	int getSampleEnd() const;

	long convertFrameToMS(const int frame);
	int convertMStoFrame(const long milliseconds);
	static float clamp(float value, float minValue, float maxValue);

private:
	int mFramesNum;
	int mSampleStart;
	int mSampleEnd;

	float mFps;
	float mMilliSecondPerFrame;
	float mMilliSecondPerHalfFrame;
};

typedef std::shared_ptr<AnimationSample> AnimationSamplePtr;
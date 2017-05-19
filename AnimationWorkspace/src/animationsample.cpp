#include <animationsample.h>

AnimationSample::AnimationSample()
	: mFramesNum(0),
	mSampleStart(0),
	mSampleEnd(0),
	mFps(0.0f),
	mMilliSecondPerFrame(0.0f),
	mMilliSecondPerHalfFrame(0.0f)
{
}


AnimationSample::~AnimationSample()
{
}

void AnimationSample::setFramePerSeconds(float fps)
{
	mFps = clamp(fps, 0.01f, 1000.f);
	mMilliSecondPerFrame = TIME_MILLISECOND / mFps;
	mMilliSecondPerHalfFrame = TIME_HALFMILLISECOND / mFps;
}

void AnimationSample::setFrameNum(int numFrames)
{
	mFramesNum = numFrames;
}

void AnimationSample::setSampleStart(int sampleStart)
{
	mSampleStart = sampleStart;
}

void AnimationSample::setSampleEnd(int sampleEnd)
{
	mSampleEnd = sampleEnd;
}

double AnimationSample::getFps() const
{
	return mFps;
}

int AnimationSample::getFrameNum() const
{
	return mFramesNum;
}

int AnimationSample::getFrameSampleNum() const
{
	return mFramesNum + 1;
}

int AnimationSample::getSampleStart() const
{
	return mSampleStart;
}

int AnimationSample::getSampleEnd() const
{
	return mSampleEnd;
}

long AnimationSample::convertFrameToMS(const int frame)
{
	return static_cast<long>(floor(frame * mMilliSecondPerFrame));
}

int AnimationSample::convertMStoFrame(const long milliseconds)
{
	return static_cast<int>(floor(milliseconds / mMilliSecondPerFrame));
}

float AnimationSample::clamp(float value, float minValue, float maxValue)
{
	if (value < minValue) {
		value = minValue;
	}
	else if (value > maxValue) {
		value = maxValue;
	}
	return value;
}

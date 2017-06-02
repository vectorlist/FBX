#include <animsample.h>
#include <fbxtool.h>


AnimSample::AnimSample()
	: mFramesNum(0),
	mSampleStart(0),
	mSampleEnd(0),
	mFrameStart(0),
	mFrameEnd(0),
	mFps(0.0f),
	mMilliSecondFrame(0.0f),
	mMilliSecondPerHalfFrame(0.0f)
{

}

AnimSample::~AnimSample()
{

}

void AnimSample::setFps(float fps)
{
	mFps = FBXTool::clamp(fps, ANIMATION_FRAME_MIN, ANIMATION_FRAME_MAX);
	mMilliSecondFrame = MILLISECOND / mFps;
	mMilliSecondPerHalfFrame = MILLIHALFSECOND / mFps;
}

void AnimSample::setFrameStart(int startFrame)
{
	mFrameStart = startFrame;
}

void AnimSample::setFrameEnd(int endFrame)
{
	mFrameEnd = endFrame;
}

void AnimSample::setName(const std::string &name)
{
	mName = name;
}

void AnimSample::setFrameNums(int frameNum)
{
	mFramesNum = frameNum;
}

void AnimSample::setSampleStart(int startSample)
{
	mSampleStart = startSample;
}

void AnimSample::setSampleEnd(int endSample)
{
	mSampleEnd = endSample;
}

float AnimSample::getFps() const
{
	return mFps;
}

int AnimSample::getSamplesFrameNum() const
{
	return mFramesNum + 1;
}

int AnimSample::getFrameStart() const
{
	return mFrameStart;
}

int AnimSample::getFrameEnd() const
{
	return mFrameEnd;
}

const std::string & AnimSample::getName() const
{
	return mName;
}

int AnimSample::getSampleStart() const
{
	return mSampleStart;
}

int AnimSample::getSampleEnd() const
{
	return mSampleEnd;
}

int AnimSample::getSampleBlock() const
{
	return mFrameStart;
}

long AnimSample::convertFrameToMilli(const int frame)
{
	return static_cast<long>(floor(frame * mMilliSecondFrame));
}

int AnimSample::convertMilliToFrame(const long millisecond)
{
	return static_cast<int>(floor(millisecond / mMilliSecondFrame));
}


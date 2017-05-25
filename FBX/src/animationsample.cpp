#include <animationsample.h>
#include <fbxtool.h>


AnimationSample::AnimationSample()
	: mFramesNum(0),
	mSampleStart(0),
	mSampleEnd(0),
	mFps(0.0f),
	mMilliSecondFrame(0.0f),
	mMilliSecondPerHalfFrame(0.0f)
{

}

AnimationSample::~AnimationSample()
{
}

void AnimationSample::setFps(float fps)
{
	mFps = FBXTool::clamp(fps, ANIMATION_FRAME_MIN, ANIMATION_FRAME_MAX);
	mMilliSecondFrame = MILLISECOND / mFps;
	mMilliSecondPerHalfFrame = MILLIHALFSECOND / mFps;

}

void AnimationSample::setName(const std::string &name)
{
	mName = name;
}

void AnimationSample::setFrameNums(int frameNum)
{
	mFramesNum = frameNum;
}

void AnimationSample::setSampleStart(int startSample)
{
	mSampleStart = startSample;
}

void AnimationSample::setSampleEnd(int endSample)
{
	mSampleEnd = endSample;
}

float AnimationSample::getFps() const
{
	return mFps;
}

int AnimationSample::getSamplesFrameNum() const
{
	return mFramesNum + 1;
}

const std::string & AnimationSample::getName() const
{
	return mName;
}

int AnimationSample::getSampleStart() const
{
	return mSampleStart;
}

int AnimationSample::getSampleEnd() const
{
	return mSampleEnd;
}

int AnimationSample::getSampleOffset() const
{
	int offset = mSampleStart - 1;
	return offset < 0 ? 0 : offset;
}

long AnimationSample::convertFrameToMilli(const int frame)
{
	return static_cast<long>(floor(frame * mMilliSecondFrame));
}

int AnimationSample::convertMilliToFrame(const long millisecond)
{
	return static_cast<int>(floor(millisecond / mMilliSecondFrame));
}


#include "animation.h"



Animation::Animation()
	: Frame(),mFps(0.0f)
{
}


Animation::~Animation()
{
}

void Animation::setFps(float fps)
{
	mFps = clamp(fps, ANIMATION_FRAME_MIN, ANIMATION_FRAME_MAX);
	mMilliPerFrame = MILLISECOND / mFps;
	mMilliPerHalfFrame = MILLIHALFSECOND / mFps;

}

float Animation::clamp(float val, float fmin, float fmax)
{
	if (val < fmin) {
		val = fmin;
	}
	else if (val > fmax){
		val = fmax;
	}
	return val;
}

void Animation::setName(const std::string &name)
{
	mName = name;
}

void Animation::setStartSample(int startSample)
{
	mStartSample = startSample;
}

void Animation::setEndSample(int endSample)
{
	mEndSample = endSample;
}

float Animation::getFps() const
{
	if (mFps == 0.0f) LOG << "warn fps  is 0.0f" << ENDN;
	return mFps;
}

int Animation::getNumFrameSamples() const
{
	return mNumFrames + 1;
}

const std::string & Animation::getName() const
{
	// TODO: insert return statement here
	return mName;
}

int Animation::getStartSample() const
{
	return mStartSample;
}

int Animation::getEndSample() const
{
	return mEndSample;
}

long Animation::convertFrameToMilli(const int frame)
{
	return static_cast<long>(floor(frame * mMilliPerFrame));
}

int Animation::covertMilliToFrame(const long milli)
{
	return static_cast<int>(floor(milli / mMilliPerFrame));
}


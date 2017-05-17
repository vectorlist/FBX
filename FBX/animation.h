#pragma once
#include <frame.h>
#include <string>
#include <log.h>
#include <memory>

#define ANIMATION_FRAME_MIN				0.f
#define ANIMATION_FRAME_MAX				1000.f
#define MILLISECOND						1000.f
#define MILLIHALFSECOND					500.f
#define ANIMATION_CURRENT_FRAME_RATE	30.f

class Animation : public Frame
{
public:
	Animation();
	~Animation();

	void setFps(float fps);

	float clamp(float val, float fmin, float fmax);

	void setName(const std::string &name);
	void setStartSample(int startSample);
	void setEndSample(int endSample);

	float getFps() const;
	int getNumFrameSamples() const;
	const std::string& getName() const;
	int getStartSample() const;
	int getEndSample() const;

	long convertFrameToMilli(const int frame);
	int covertMilliToFrame(const long milli);

private:
	float mFps;
	float mMilliPerFrame;
	float mMilliPerHalfFrame;
	std::string mName;
	int mStartSample;
	int mEndSample;
	
};

typedef std::shared_ptr<Animation> AnimationPtr;
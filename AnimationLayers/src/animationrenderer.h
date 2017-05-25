#pragma once

#include <animationscene.h>
#include <animationsample.h>
#include <vector>

#define GET_TIME()		timeGetTime()
typedef unsigned long uLong;

class AnimationRenderer
{
public:
	AnimationRenderer();
	~AnimationRenderer();

	long mLastTime;

	void playAnimation();
	void update();

	bool isRunning;
	bool isSampleChanged;

	AnimationSample* currentSample();
	uint32_t mCurrentSampleID;

	void changeSampleID();

	std::vector<AnimationSamplePtr> mSamples;
	AnimationScenePtr mScene;
};


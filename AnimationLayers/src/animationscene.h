#pragma once

#include <Windows.h>
#include <memory>

class AnimationSample;
class AnimationScene
{
public:
	AnimationScene();

	bool startAnimation(
		long globalStartTime,
		long animationStartTime,
		long animationEndTime);

	void update(AnimationSample *sample, const long globalTime);

	void clampLocalFrame();

	long mGlobalStartTime;
	long mLocalCurrentTime;
	long mAnimationStartTime;
	long mAnimationEndTime;

	int mLocalFrame;

	bool isAnimating;
};

typedef std::shared_ptr<AnimationScene> AnimationScenePtr;
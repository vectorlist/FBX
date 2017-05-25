#include <animationscene.h>
#include <log.h>
#include <animationsample.h>

AnimationScene::AnimationScene()
	: mGlobalStartTime(0),
	mLocalCurrentTime(0),
	mAnimationStartTime(0),
	mAnimationEndTime(0),
	isAnimating(false)
{

}

bool AnimationScene::startAnimation(
	long globalStartTime,
	long animationStartTime,
	long animationEndTime)
{
	if (animationEndTime <= 0) {
		return false;
	}
	mGlobalStartTime = globalStartTime;

	mAnimationStartTime = animationStartTime;
	mAnimationEndTime = animationEndTime;

	isAnimating = true;
	return true;
}

void AnimationScene::update(AnimationSample* sample,const long globalTime)
{
	mLocalCurrentTime = globalTime - mGlobalStartTime;
	
	clampLocalFrame();
	mLocalFrame = sample->convertMStoFrame(mLocalCurrentTime);
	LOG << "sample time : " << mLocalCurrentTime << " frame : " << mLocalFrame << ENDN;

}

void AnimationScene::clampLocalFrame()
{
	long length = mAnimationEndTime - mAnimationStartTime;
	mLocalCurrentTime = mLocalCurrentTime % length;
}

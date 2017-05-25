#include <animationrenderer.h>
#include <log.h>

#define SAMPLE_START	0
#define SAMPLE_END		30
#define FPS				60

AnimationRenderer::AnimationRenderer()
	: mLastTime(0), isRunning(false), isSampleChanged(false), mCurrentSampleID(0)
{
	mScene = AnimationScenePtr(new AnimationScene());
	
	int sampleStackNum = 3;
	
	auto sample00 = AnimationSamplePtr(new AnimationSample());
	sample00->setFramePerSeconds(FPS);
	sample00->setSampleStart(SAMPLE_START);
	sample00->setSampleEnd(SAMPLE_END);
	sample00->setFrameNum(SAMPLE_END - SAMPLE_START - 1);
	mSamples.push_back(sample00);

	auto sample01 = AnimationSamplePtr(new AnimationSample());
	sample01->setFramePerSeconds(100);
	sample01->setSampleStart(0);
	sample01->setSampleEnd(70);
	sample01->setFrameNum(70 - 0 - 1);
	mSamples.push_back(sample01);

	auto sample02 = AnimationSamplePtr(new AnimationSample());
	sample02->setFramePerSeconds(20);
	sample02->setSampleStart(0);
	sample02->setSampleEnd(40);
	sample02->setFrameNum(40 - 0 - 1);
	mSamples.push_back(sample02);
}

AnimationRenderer::~AnimationRenderer()
{

}

AnimationSample* AnimationRenderer::currentSample()
{
	if(mSamples.size() == NULL) return NULL;
	return mSamples[mCurrentSampleID].get();
}

void AnimationRenderer::changeSampleID()
{
	if (mCurrentSampleID < mSamples.size() - 1) {
		mCurrentSampleID++;
	}
	else {
		mCurrentSampleID = 0;
	}
	LOG << "--------------------- change sample ID ------------------------ : "
		<< mCurrentSampleID << ENDN;
	isSampleChanged = true;
}

void AnimationRenderer::playAnimation()
{
	if (!isRunning || isSampleChanged)
	{
		AnimationSample* sample = currentSample();
		if (!sample) return;
		long start = sample->convertFrameToMS(sample->getSampleStart());
		long end = sample->convertFrameToMS(sample->getSampleEnd());

		mScene->startAnimation(GET_TIME(), start, end);
		isRunning = true;
		isSampleChanged = false;
	}
	else {
		update();
	}
}

void AnimationRenderer::update()
{
	long now = GET_TIME();
	mScene->update(currentSample(), now);
	mLastTime = now;
}

#include <animlayer.h>
#include <log.h>

AnimLayer::AnimLayer()
	: mFps(ANIMATION_DEFAULT_FRAME_RATE)
{
}

void AnimLayer::createLayers(FbxImporter* importer)
{
	FbxTime::EMode fbxTimeMode;
	if (importer->GetFrameRate(fbxTimeMode))
	{
		mFps = FbxTime::GetFrameRate(fbxTimeMode);
	}

	long baseStartTime = 0;
	long baseEndTime = 0;

	for (int i = 0; i < importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* take = importer->GetTakeInfo(i);
		animsample_ptr sample = std::make_shared<AnimSample>();
		
		sample->setName(take->mName.Buffer());
		sample->setFps(mFps);

		const long startTime = take->mLocalTimeSpan.GetStart().GetMilliSeconds();
		const long endTime = take->mLocalTimeSpan.GetStop().GetMilliSeconds();
		sample->setFrameStart(sample->convertMilliToFrame(startTime));
		sample->setFrameEnd(sample->convertMilliToFrame(endTime));
		sample->setSampleStart(0);
		sample->setSampleEnd(sample->getFrameEnd() - sample->getFrameStart());
		sample->setFrameNums(sample->getSampleEnd() - 1);
		

		baseStartTime = std::min<long>(baseStartTime, startTime);
		baseEndTime = std::max<long>(baseEndTime, endTime);
		debugSample(sample.get());
		
		mSamples.push_back(std::move(sample));
	}

	auto sample = std::make_shared<AnimSample>();
	sample->setName("base animation sample");
	sample->setFps(mFps);
	sample->setSampleStart(sample->convertMilliToFrame(baseStartTime));
	sample->setSampleEnd(sample->convertMilliToFrame(baseEndTime));
	int numFrame = sample->getSampleEnd() - sample->getSampleStart() - 1;
	sample->setFrameNums(numFrame);
	
	mBaseSample = sample;
	debugSample(sample.get());
}

AnimSample* AnimLayer::getSample(unsigned int index)
{
	return mSamples[index].get();
}

AnimSample * AnimLayer::getBaseSample()
{
	return mBaseSample.get();
}

int AnimLayer::getSamplesNum() const
{
	return mSamples.size();
}

float AnimLayer::getFps()
{
	return mFps;
}

void AnimLayer::debugSample(AnimSample* sample)
{
	LOG << " ------------------------------------------------------------------- " << ENDN;
	LOG << "Animation Sample Name : " << sample->getName() << ENDN;
	LOG << "Frame Start : " << sample->getFrameStart() << " Frame End : " << sample->getFrameEnd() << ENDN;
	LOG << "Time Start : " << sample->getSampleStart() << " Time End : " << sample->getSampleEnd() << ENDN;
	LOG << "Sample Frame Num : " << sample->getSamplesFrameNum() << ENDN;
	LOG << " ------------------------------------------------------------------- " << ENDN;
}

#include <animlayer.h>
#include <log.h>

AnimLayer::AnimLayer()
	: mIndex(0), mNumSamples(0)
{
}

void AnimLayer::createLayers(FbxImporter* importer)
{
	FbxTime::EMode fbxTimeMode;
	float fps;
	if (importer->GetFrameRate(fbxTimeMode))
	{
		fps = FbxTime::GetFrameRate(fbxTimeMode);
	}

	for (int i = 0; i < importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* Take = importer->GetTakeInfo(i);
		animsample_ptr Sample = std::make_shared<AnimSample>();
		
		FbxTime Start = Take->mLocalTimeSpan.GetStart();
		FbxTime End = Take->mLocalTimeSpan.GetStop();
		FbxTime Length =  End.GetFrameCount(FRAME_24) - Start.GetFrameCount(FRAME_24) + 1;

		Sample->mName = Take->mName.Buffer();
		Sample->mStart = Start.GetMilliSeconds() / 1000.f;
		Sample->mEnd = End.GetMilliSeconds() / 1000.f;
		Sample->mDuration = (End.GetMilliSeconds() - Start.GetMilliSeconds()) / 1000.f;

		Sample->mFrameStart = Start.GetFrameCount(FRAME_24);
		Sample->mFrameEnd = End.GetFrameCount(FRAME_24);
		Sample->mFrameCount = Length.Get();

		
		mSamples.push_back(Sample);
		mSamplesNames.push_back(Sample->mName.c_str());
		mNumSamples++;
	}

}

AnimSample* AnimLayer::getSample(int index)
{
	return mSamples[index].get();
}


std::vector<const char*>& AnimLayer::GetSamplesNames()
{
	return mSamplesNames;
}

void AnimLayer::debugSample(AnimSample* sample)
{
	//LOG << " ------------------------------------------------------------------- " << ENDN;
	//LOG << "Animation Sample Name : " << sample->getName() << ENDN;
	//LOG << "Frame Start : " << sample->getFrameStart() << " Frame End : " << sample->getFrameEnd() << ENDN;
	//LOG << "Time Start : " << sample->getSampleStart() << " Time End : " << sample->getSampleEnd() << ENDN;
	//LOG << "Sample Frame Num : " << sample->getSamplesFrameNum() << ENDN;
	//LOG << " ------------------------------------------------------------------- " << ENDN;
}

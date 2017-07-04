#include <animlayer.h>
#include <log.h>

AnimLayer::AnimLayer()
	: mCurrentSampleIndex(0), mNumSamples(0)
{
}

void AnimLayer::createLayers(FbxImporter* importer)
{
	
	for (int i = 0; i < importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* Take = importer->GetTakeInfo(i);
		animsample_ptr Sample = std::make_shared<AnimSample>();
		
		FbxTime Start = Take->mLocalTimeSpan.GetStart();
		FbxTime End = Take->mLocalTimeSpan.GetStop();
		FbxTime Duration = Take->mLocalTimeSpan.GetDuration();
		FbxTime Length =  End.GetFrameCount(FRAME_24) - Start.GetFrameCount(FRAME_24) + 1;
		
		Sample->mName = Take->mName.Buffer();
		Sample->mStart = Start.GetMilliSeconds() / 1000.f;
		Sample->mEnd = End.GetMilliSeconds() / 1000.f;
		Sample->mDuration = (End.GetMilliSeconds() - Start.GetMilliSeconds()) / 1000.f;

		Sample->mFrameStart = Start.GetFrameCount(FRAME_24);
		Sample->mFrameEnd = End.GetFrameCount(FRAME_24);
		Sample->mFrameCount = Length.Get();
		Sample->mIndex = mNumSamples;

		auto Mode = Start.GetGlobalTimeMode();
		Sample->mFps = Start.GetFrameRate(Mode);

		
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


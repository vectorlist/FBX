#include <animationlayer.h>

void AnimationLayers::createLayers(FbxImporter* importer)
{
	layers.clear();
	for (int i = 0; i < importer->GetAnimStackCount(); ++i)
	{
		FbxTakeInfo* take = importer->GetTakeInfo(i);
		LayerInfo layer;
		layer.name = take->mName.Buffer();
		layer.timeStart = take->mLocalTimeSpan.GetStart().GetMilliSeconds();
		layer.timeEnd = take->mLocalTimeSpan.GetStop().GetMilliSeconds();
		layer.length = layer.timeEnd - layer.timeStart;

		bool debugInfo = true;
		if (debugInfo)
		{
			LOG << " ------------------------------------------------------------------- " << ENDN;
			LOG << "created anim layer name : " << layer.name << ENDN;
			LOG << "start : " << layer.timeStart << " end : " << layer.timeEnd << ENDN;
			LOG << "layer length : " << layer.length << ENDN;
			LOG << " ------------------------------------------------------------------- " << ENDN;
		}
		AnimationSamplePtr sample = createSample(layer);
		samples.push_back(std::move(sample));
		layers.push_back(layer);
		
	}
	
	auto g = 100;
}

AnimationSample* AnimationLayers::getSample(unsigned int index)
{
	return samples[index].get();
}

const LayerInfo& AnimationLayers::getLayer(unsigned int index) const
{
	return layers[index];
}

int AnimationLayers::getTotalSampleFrameNum() const
{
	return mTotalSampleNum;
}

int AnimationLayers::size() const
{
	return layers.size();
}

AnimationSamplePtr AnimationLayers::createSample(const LayerInfo &info)
{
	AnimationSamplePtr sample = AnimationSamplePtr(new AnimationSample);

	float frameRate = ANIMATION_CURRENT_FRAME_RATE;

	sample->setName(info.name);
	sample->setFps(frameRate);

	const long startTime = info.timeStart;
	const long endTime = info.timeEnd;
	int sampleStart = sample->convertMilliToFrame(startTime);
	int sampleEnd = sample->convertMilliToFrame(endTime);
	sample->setSampleStart(sampleStart);
	sample->setSampleEnd(sampleEnd);

	//get numFrame
	int frameNum = sample->getSampleEnd() - sample->getSampleStart() - 1;
	sample->setFrameNums(frameNum);

	bool debugAnimationInfo = true;
	if (debugAnimationInfo)
	{
		LOG << "created sample name : " << sample->getName() << " frame per second : " << sample->getFps() << ENDN;
		LOG << "start : " << sample->getSampleStart() << " end : " << sample->getSampleEnd() << ENDN;
		LOG << "sameple num : " << sample->getSamplesFrameNum() << ENDN;
	}

	//amount sampleNum
	mTotalSampleNum += sample->getSamplesFrameNum();
	return std::move(sample);
}

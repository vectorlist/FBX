#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <memory>
#include <animsample.h>
#include <algorithm>
#include <map>

#define FRAME_24		FbxTime::eFrames24
#define FRAME_30		FbxTime::eFrames30

class AnimLayer
{
public:
	AnimLayer();
	
	void createLayers(FbxImporter* importer);
	AnimSample* getSample(int index);
	
	std::vector<const char*>& GetSamplesNames();
	static void debugSample(AnimSample* sample);

	int mIndex;
	int mNumSamples;
private:
	std::vector<animsample_ptr>	mSamples;
	std::vector<const char*>	mSamplesNames;
};


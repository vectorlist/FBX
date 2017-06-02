#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <memory>
#include <animsample.h>
#include <algorithm>
#include <map>

class AnimLayer
{
public:
	AnimLayer();

	void createLayers(FbxImporter* importer);

	AnimSample*					getSample(unsigned int index);
	std::vector<const char*>	getSamplesNames();
	AnimSample*					getBaseSample();
	int							getSamplesNum() const;
	float						getFps();
	int							index;
	static void debugSample(AnimSample* sample);
private:

	std::vector<animsample_ptr>	mSamples;
	std::vector<const char*>	mSamplesNames;
	animsample_ptr				mBaseSample;

	//For Gui
	float						mFps;
};


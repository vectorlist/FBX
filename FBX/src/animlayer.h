#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <memory>
#include <animsample.h>
#include <algorithm>

class AnimLayer
{
public:
	AnimLayer();

	void createLayers(FbxImporter* importer);

	AnimSample* getSample(unsigned int index);
	AnimSample* getBaseSample();
	int getSamplesNum() const;
	float getFps();
	
	static void debugSample(AnimSample* sample);
private:

	std::vector<animsample_ptr> mSamples;
	animsample_ptr mBaseSample;

	float mFps;
};


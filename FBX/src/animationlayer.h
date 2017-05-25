#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <log.h>
#include <memory>
#include <map>
#include <animationsample.h>

struct LayerInfo
{
	std::string name;
	//MilliSeconds
	long length;
	long timeStart;
	long timeEnd;


	bool operator<(const LayerInfo& other)const
	{
		return (this->length < other.length);
	}
};

class AnimationLayers
{
public:
	AnimationLayers() : mTotalSampleNum(0){}

	LayerInfo& operator[](int index);
	void createLayers(FbxImporter* importer);

	//TODO get sample by name or id
	AnimationSample* getSample(unsigned int index);
	const LayerInfo& getLayer(unsigned int index) const;
	int getTotalSampleFrameNum() const;
	int size() const;
private:
	AnimationSamplePtr createSample(const LayerInfo& info);

	std::vector<LayerInfo> layers;
	std::vector<AnimationSamplePtr> samples;
	//std::map<LayerInfo, AnimationSamplePtr> layerSamples;
	int mTotalSampleNum;
};

inline LayerInfo& AnimationLayers::operator[](int index)
{
	assert(layers.size() < index);
	return layers[index];
}

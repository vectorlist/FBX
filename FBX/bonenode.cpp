#include "BoneNode.h"
#include <log.h>

uint32_t BoneNode::globalID = 0;

BoneNode::BoneNode()
	: mID(globalID++), mName("null"), alloactedTrackSize(0)
{

}

BoneNode::~BoneNode()
{

}

void BoneNode::setName(const std::string & name)
{
	mName = name;
}

const std::string& BoneNode::getName() const
{
	return mName;
}

uint32_t BoneNode::getID()
{
	return mID;
}

void BoneNode::resetGlobalID()
{
	globalID = 0;
}

void BoneNode::allocateTracks(int frameNum)
{
	
	mPositionTrack = TrackVec3Ptr(new TrackVec3(frameNum));
	mScaleTrack = TrackVec3Ptr(new TrackVec3(frameNum));
	mQuarternionTrack = TrackQuaternionPtr(new TrackQuaternion(frameNum));
	alloactedTrackSize = frameNum;
	LOG << "allocated keys : " << alloactedTrackSize << ENDN;
}

void BoneNode::addPositionKey(const KeyVector &key)
{
	mPositionTrack->addKey(key);
}

KeyVector & BoneNode::getPositionKey(int sample)
{
	return mPositionTrack->getKey(sample);
}

void BoneNode::addScaleKey(const KeyVector &key)
{
	mScaleTrack->addKey(key);
}

KeyVector & BoneNode::getScaleKey(int sample)
{
	return mScaleTrack->getKey(sample);
}

void BoneNode::addRotationKey(const KeyQuaternion &key)
{
	mQuarternionTrack->addKey(key);
}

KeyQuaternion& BoneNode::getRotationKey(int sample)
{
	return mQuarternionTrack->getKey(sample);
}


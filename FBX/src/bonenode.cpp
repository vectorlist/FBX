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

FbxAMatrix& BoneNode::getGlobalTransfrom()
{
	//it can handle outside
	return mGlobalTransform;
}

const FbxAMatrix& BoneNode::getGlobalMatrix() const
{
	// TODO: insert return statement here
	return mGlobalTransform;
}

FbxAMatrix& BoneNode::getInveseLocalTransfrom()
{
	// TODO: insert return statement here
	return mInverseTransform;
}

const FbxAMatrix& BoneNode::getInveseLocalMatrix() const
{
	// TODO: insert return statement here
	return mInverseTransform;
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

void BoneNode::addPositionKey(const KeyVec3 &key)
{
	mPositionTrack->addKey(key);
}

KeyVec3& BoneNode::getPositionKey(int sample)
{
	return mPositionTrack->getKey(sample);
}

TrackVec3* BoneNode::getPositionTrack()
{
	return mPositionTrack.get();
}

void BoneNode::addScaleKey(const KeyVec3 &key)
{
	mScaleTrack->addKey(key);
}

KeyVec3& BoneNode::getScaleKey(int sample)
{
	return mScaleTrack->getKey(sample);
}

TrackVec3* BoneNode::getScaleTrack()
{
	return mScaleTrack.get();
}

void BoneNode::addRotationKey(const KeyQuaternion &key)
{
	mQuarternionTrack->addKey(key);
}

KeyQuaternion& BoneNode::getRotationKey(int sample)
{
	return mQuarternionTrack->getKey(sample);
}

TrackQuaternion* BoneNode::getQuaternionTrack()
{
	return mQuarternionTrack.get();
}


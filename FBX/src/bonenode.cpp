#include "BoneNode.h"
#include <log.h>


BoneNode::BoneNode()
	: mName("null"), alloactedTrackSize(0)
{

}

BoneNode::~BoneNode()
{

	//LOG << " delete : " << mName << ENDN;
}

void BoneNode::setName(const std::string & name)
{
	mName = name;
}

const std::string& BoneNode::getName() const
{
	return mName;
}

FbxAMatrix& BoneNode::getGlobalTransfrom()
{
	return mGlobalTransform;
}

const FbxAMatrix& BoneNode::getGlobalTransfrom() const
{
	return mGlobalTransform;
}

FbxAMatrix& BoneNode::getInveseLocalTransfrom()
{
	return mInverseTransform;
}

const FbxAMatrix& BoneNode::getInveseLocalTransfrom() const
{
	return mInverseTransform;
}

void BoneNode::setInverseLocalTransfrom(const FbxAMatrix &invTransform)
{
	mInverseTransform = invTransform;
}

void BoneNode::setInheritScale(bool inheritScale)
{
	mInheritScale = inheritScale;
}

bool BoneNode::getInheritScale()
{
	return mInheritScale;
}

void BoneNode::allocateTracks(int KeyNums)
{
	mPositionTrack = TrackVec3Ptr(new TrackVec3(KeyNums));
	mScaleTrack = TrackVec3Ptr(new TrackVec3(KeyNums));
	mQuarternionTrack = TrackQuaternionPtr(new TrackQuaternion(KeyNums));
	alloactedTrackSize = KeyNums;
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


#include "BoneNode.h"
#include <log.h>


BoneNode::BoneNode()
	: mName("null"), mAllocatedSize(0)
{

}

BoneNode::~BoneNode()
{

}

void BoneNode::SetName(const std::string &name)
{
	mName = name;
}

const std::string& BoneNode::GetName() const
{
	return mName;
}

Matrix4x4& BoneNode::GetGlobalTransform()
{
	return mGlobalTransform;
}

const Matrix4x4& BoneNode::GetGlobalTransform() const
{
	return mGlobalTransform;
}

Matrix4x4& BoneNode::GetInveseLocalTransfrom()
{
	return mInverseTransform;
}

const Matrix4x4& BoneNode::GetInveseLocalTransfrom() const
{
	return mInverseTransform;
}

void BoneNode::SetGlobalTransform(const Matrix4x4 &other)
{
	mGlobalTransform = other;
}

void BoneNode::SetInverseLocalTransfrom(const Matrix4x4 &other)
{
	mInverseTransform = other;
}

void BoneNode::SetInheritScale(bool inheritScale)
{
	mInheritScale = inheritScale;
}

bool BoneNode::GetInheritScale()
{
	return mInheritScale;
}

void BoneNode::AllocateTracks(int frameNum)
{
	mPositionTrack = VectorTrackPtr(new VectorTrack(frameNum));
	mScaleTrack = VectorTrackPtr(new VectorTrack(frameNum));
	mRotationTrack = QautTrackPtr(new QautTrack(frameNum));
	mAllocatedSize = frameNum;
}

void BoneNode::AddPositionKey(const VectorKey &key)
{
	mPositionTrack->AddKey(key);
}

VectorKey& BoneNode::GetPositionKey(int frame)
{
	return mPositionTrack->GetKey(frame);
}

VectorTrack* BoneNode::GetPositionTrack()
{
	return mPositionTrack.get();
}

void BoneNode::AddScaleKey(const VectorKey &key)
{
	mScaleTrack->AddKey(key);
}

VectorKey& BoneNode::GetScaleKey(int frame)
{
	return mScaleTrack->GetKey(frame);
}

VectorTrack* BoneNode::GetScaleTrack()
{
	return mScaleTrack.get();
}

void BoneNode::AddRotationKey(const QuatKey &key)
{
	mRotationTrack->AddKey(key);
}

QuatKey& BoneNode::GetRotationKey(int frame)
{
	return mRotationTrack->GetKey(frame);
}

QautTrack* BoneNode::GetRotationTrack()
{
	return mRotationTrack.get();
}

void BoneNode::SetNextGlobalTransform(const Matrix4x4 &other)
{
	mNextGlobalTransform = other;
}

Matrix4x4& BoneNode::GetNextGlobalTransform()
{
	return mNextGlobalTransform;
}

const Matrix4x4& BoneNode::GetNextGlobalTransform() const
{
	return mNextGlobalTransform;
}


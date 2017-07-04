#pragma once

#include <nodetree.h>
#include <string>
#include <track.h>

class BoneNode : public NodeTreeItem<BoneNode>
{
public:
	BoneNode();
	~BoneNode();

	void					SetName(const std::string &name);
	const std::string&		GetName() const;

	Matrix4x4&				GetGlobalTransform();
	const Matrix4x4&		GetGlobalTransform() const;
	Matrix4x4&				GetInveseLocalTransfrom();
	const Matrix4x4&		GetInveseLocalTransfrom() const;

	void					SetGlobalTransform(const Matrix4x4& other);
	void					SetInverseLocalTransfrom(const Matrix4x4 &other);
	void					SetInheritScale(bool inheritSacle);
	bool					GetInheritScale();

	void					AllocateTracks(int KeyNums);

	void					AddPositionKey(const VectorKey& key);
	VectorKey&				GetPositionKey(int frame);
	VectorTrack*			GetPositionTrack();

	void					AddScaleKey(const VectorKey& key);
	VectorKey&				GetScaleKey(int frame);
	VectorTrack*			GetScaleTrack();

	void					AddRotationKey(const QuatKey& key);
	QuatKey&				GetRotationKey(int frame);
	QautTrack*				GetRotationTrack();

	void					SetNextGlobalTransform(const Matrix4x4 &other);
	Matrix4x4&				GetNextGlobalTransform();
	const Matrix4x4&		GetNextGlobalTransform() const;

private:
	VectorTrackPtr			mPositionTrack;
	VectorTrackPtr			mScaleTrack;
	QautTrackPtr			mRotationTrack;

	Matrix4x4				mGlobalTransform;
	Matrix4x4				mInverseTransform;
	bool					mInheritScale;

	Matrix4x4				mNextGlobalTransform;

	std::string				mName;
	int						mAllocatedSize;
};


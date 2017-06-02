#pragma once

#include <nodetree.h>
#include <string>
#include <fbxsdk/core/math/fbxaffinematrix.h>
#include <track.h>

using namespace fbxsdk;
class TrackContainer;
class BoneNode : public NodeTreeItem<BoneNode>
{
public:
	BoneNode();
	~BoneNode();

	void setName(const std::string &name);
	const std::string& getName() const;
	//uint32_t id();

	FbxAMatrix& getGlobalTransfrom();
	const FbxAMatrix& getGlobalTransfrom() const;
	FbxAMatrix& getInveseLocalTransfrom();
	const FbxAMatrix& getInveseLocalTransfrom() const;

	void setInverseLocalTransfrom(const FbxAMatrix &invTransform);
	void setInheritScale(bool inheritSacle);
	bool getInheritScale();
private:
	std::string mName;

	static uint32_t globalID;
public:
	FbxAMatrix mGlobalTransform;
	FbxAMatrix mInverseTransform;
	bool mInheritScale;
	/*------------- TRACK ----------------*/
	TrackContainer* mTrack;
	void allocateTracks(int frameNum);

	//Position
	void addPositionKey(const KeyVec3& key);
	KeyVec3& getPositionKey(int sample);
	TrackVec3* getPositionTrack();

	//scale
	void addScaleKey(const KeyVec3& key);
	KeyVec3& getScaleKey(int sample);
	TrackVec3* getScaleTrack();

	//rotation quaternion
	void addRotationKey(const KeyQuaternion& key);
	KeyQuaternion& getRotationKey(int sample);
	TrackQuaternion* getQuaternionTrack();
private:
	TrackVec3Ptr mPositionTrack;
	TrackVec3Ptr mScaleTrack;
	TrackQuaternionPtr mQuarternionTrack;
	
	int alloactedTrackSize;
};


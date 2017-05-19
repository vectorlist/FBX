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
	uint32_t getID();

	FbxAMatrix& getGlobalTransfrom();
	const FbxAMatrix& getGlobalMatrix() const;
	FbxAMatrix& getInveseLocalTransfrom();
	const FbxAMatrix& getInveseLocalMatrix() const;
	static void resetGlobalID();
private:
	std::string mName;
	uint32_t mID;

	static uint32_t globalID;
public:
	FbxAMatrix mGlobalTransform;
	FbxAMatrix mInverseTransform;

	/*------------- TRACK ----------------*/
	TrackContainer* mTrack;
	//replace to container
	//allocate Datas tracks
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


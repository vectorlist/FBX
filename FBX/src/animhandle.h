#pragma once

#include <animsample.h>
#include <memory>
#include <fbxsdk.h>

class KeyQuaternion;
class KeyVec3;
class TrackQuaternion;
class TrackVec3;
class Node;
class BoneNode;
class AnimHandle
{
public:
	AnimHandle();
	~AnimHandle();


	bool startAnimation(
		long globalStartTime,
		long animationStartTime,
		long animationEndTime);

	bool isAnimating() const;
	bool isPause() const;
	void setLoop(bool loop);

	
	void updateNodes(Node* node,const long globalTime);
	
	void evalNodes(
		int sample,
		BoneNode* pBoneNode,
		const FbxAMatrix &parentGlobalScale,
		const FbxAMatrix &parentGlobalRoataion);

	void clampLocalCurrentTime();

	/*BUILT IN CALC FUNC*/
	void interpolatePosition(
		int sample,
		TrackVec3* positionTrack,
		KeyVec3& output);

	void interpolateScale(
		int sample,
		TrackVec3* scaleTrack,
		KeyVec3& output);

	void interpolateRoation(
		int sample,
		TrackQuaternion* rotationTrack,
		KeyQuaternion& output);

	float normalize(float last, float next, float current);

	long getLocalTime() const;
	int getLocalFrame() const;
	int getGlobalFrame() const;
	void debugSampleKey(const KeyVec3& last, const KeyVec3& next);
private:
	long mGlobalStartTime;
	long mLocalCurrentTime;
	long mAnimStartTime;
	long mAnimEndTime;

	int mLocalFrame;
	int mGlobalFrame;

	bool mIsLoop;
	bool mIsAnimating;
	bool mIsPause;
};

typedef std::shared_ptr<AnimHandle> animhandle_ptr;

#pragma once

#include <animationsample.h>
#include <memory>
#include <fbxsdk.h>

class KeyQuaternion;
class KeyVec3;
class TrackQuaternion;
class TrackVec3;
class Node;
class BoneNode;
class AnimationScene
{
public:
	AnimationScene();
	~AnimationScene();


	bool startAnimation(
		long globalStartTime,
		long animationStartTime,
		long animationEndTime);

	bool isAnimating() const;
	bool isPause() const;
	void setLoop(bool loop);

	//test
	//void updateNode(Node* node, const long globalTime);
	void updateNode(Node* node,const long globalTime);
	//recursive bone nodes and get matrix
	void evalNodes(
		int sample,
		BoneNode* pBoneNode,
		const FbxAMatrix &parentGlobalScale,
		const FbxAMatrix &parentGlobalRoataion);

	void evalFrame(int sample, BoneNode* pNode);
	
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

	void debugSampleKey(const KeyVec3& last, const KeyVec3& next);
private:
	long mGlobalStartTime;
	long mLocalCurrentTime;
	long mAnimStartTime;
	long mAnimEndTime;

	bool mIsLoop;
	bool mIsAnimating;
	bool mIsPause;
};

typedef std::shared_ptr<AnimationScene> AnimationScenePtr;

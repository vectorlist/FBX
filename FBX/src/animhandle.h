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


	void updateNodes(Node* node,const float delta);
	
	void evalNodes(
		float sample,
		BoneNode* pBoneNode,
		const FbxAMatrix &parentGlobalScale,
		const FbxAMatrix &parentGlobalRoataion);

	//Replace FrameCoundt to Float Millisec0nds
	FbxVector4 interpolatePosition(
		float sample,
		TrackVec3* Track);

	FbxVector4 interpolateScale(
		float sample,
		TrackVec3* Track);

	KeyQuaternion interpolateRotation(
		float SampleTime, TrackQuaternion* Track);

	FbxVector4 Lerp(
		const FbxVector4 &Current,
		const FbxVector4 &Next, 
		float Percent);
	void Lerp(	float NormalizeTime,
				const KeyQuaternion &Current,
				const KeyQuaternion &Next,
				KeyQuaternion &Result);
	void Slerp(
		const float normalizedTime,
		const KeyQuaternion &key,
		const KeyQuaternion &nextKey,
		KeyQuaternion &result);
private:
	bool mIsAnimating;
	float mGlobalTime;
	float mSpeed;
};

typedef std::shared_ptr<AnimHandle> animhandle_ptr;

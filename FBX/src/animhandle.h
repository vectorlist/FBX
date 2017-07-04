#pragma once

#include <animsample.h>
#include <memory>
#include <vec3f.h>
#include <quaternion.h>

class QuatKey;
class VectorKey;
class QautTrack;
class VectorTrack;
class Node;
class BoneNode;
class AnimHandle
{
public:
	AnimHandle();
	~AnimHandle();


	void		UpdateNodes(Node* node,const float delta);
	
	void		EvalNodes(
					float				sample,
					int					frame,
					BoneNode*			pBoneNode,
					const Matrix4x4&	parentScale,
					const Matrix4x4&	parentRot);

	void		EvalNextNodes(
		float				sample,
		int					frame,
		BoneNode*			pBoneNode,
		const Matrix4x4&	parentScale,
		const Matrix4x4&	parentRot);



	vec3f		InterpolatePosition(float sample, int frame,VectorTrack *track);
	vec3f		InterpolateScale(float sample, int frame,VectorTrack* track);
	Quaternion	InterpolateRotation(float sample, int frame,QautTrack* track);
	float		GetLocalTime(float globalTime, AnimSample* sample);
	int			GetLocalFrame(float sample, VectorTrack* Track);
	float		GetLocalFactor(int Frame, AnimSample* Sample);

	//Test Own CallBack Function(static only)
	float		mWeight = 0.0f;
	int			mLocalKeyFrame;
private:
	bool		mIsAnimating;
	float		mGlobalTime;
	float		mSpeed;
	bool		mIsBlend = false;
};

class AnimHandlePtr;
typedef std::shared_ptr<AnimHandle> animhandle_ptr;

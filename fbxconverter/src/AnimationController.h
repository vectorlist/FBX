#pragma once

#include <string>
#include "MeshFwdDecl.h"

class QuaternionKey;
class VectorKey;
class QuaternionTrack;
class VectorTrack;

class AnimationController
{
public :

	AnimationController();

	~AnimationController(){}

	bool IsAnimating()
	{
		return m_isAnimating;
	}

	void SetLooping(
		bool looping
		)
	{
		m_isLooping = looping;
	}

	bool IsLooping()
	{
		return m_isLooping;
	}

	bool IsPaused()
	{
		return m_isPaused;
	}

	bool StartAnimation(
		long globalStartTime,
		long animStartTime,
		long animEndTime
		);

	/**
		Resume playing a paused animation
	*/
	bool ResumeAnimation();

	void PauseAnimation();

	void StopAnimation();

	void Update(
		MeshPtr m_mesh,
		const long globalTime
		);

	private:
		void ClampTime();

	void PrepareBoneHierarcy(
		int sample,
		BoneNode* boneNode,
		const FbxAMatrix &parentGlobalScaleMatrix,
		const FbxAMatrix &parentGlobalRotationMatrix
		);

	void InterpolatePosition(
		int sample,
		boost::shared_ptr<VectorTrack> positionTrack,
		VectorKey &result
		);

	void InterpolateRotation(
		int sample,
		boost::shared_ptr<QuaternionTrack> rotationTrack,
		QuaternionKey &result
		);

	void InterpolateScale(
		int sample,
		boost::shared_ptr<VectorTrack> scaleTrack,
		VectorKey &result
		);

	void Lerp(
		const float time, 
		const VectorKey &key,
		const VectorKey &nextKey,
		VectorKey &result
		);

	void Lerp(
		const float time, 
		const QuaternionKey &key,
		const QuaternionKey &nextKey,
		QuaternionKey &result
		);

	void Slerp(
		const float time, 
		const QuaternionKey &key,
		const QuaternionKey &nextKey,
		QuaternionKey &result
		);

		long m_globalStartTime;
		long m_localCurrentTime;
		long m_animStartTime;
		long m_animEndTime;

		bool m_isLooping;
		bool m_isAnimating;
		bool m_isPaused;
	
};


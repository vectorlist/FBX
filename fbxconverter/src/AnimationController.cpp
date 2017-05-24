#include "AnimationController.h"

#include "BoneNode.h"
#include "MeshNode.h" // FIXME why do I need this???
#include "QuaternionKey.h"
#include "VectorKey.h"
#include "QuaternionTrack.h"
#include "VectorTrack.h"
#include "MathsUtils.h"
#include "AnimationInfo.h"
#include "fmesh.h"


AnimationController::AnimationController()
	: 
	m_globalStartTime(0),
	m_localCurrentTime(0),
	m_animStartTime(0),
	m_animEndTime(0),
	m_isLooping(true),
	m_isAnimating(false),
	m_isPaused(false)
{


}

bool AnimationController::StartAnimation(
	long globalStartTime,
	long animStartTime,
	long animEndTime
	)
{
	if(animEndTime == 0)
	{
		return false;
	}

	m_globalStartTime = globalStartTime;

	// Calculate the start and end times for this animation
	m_animStartTime = animStartTime;
	m_animEndTime = animEndTime;

	m_isAnimating = true;
	m_isPaused = false;

	return true;
}

bool AnimationController::ResumeAnimation()
{
	if(!m_isPaused)
	{
		return false;
	}

	m_isAnimating = true;
	m_isPaused = false;

	return true;
}

void AnimationController::PauseAnimation()
{
	if(m_isAnimating)
	{
		m_isAnimating = false;
		m_isPaused = true;
	}
}

void AnimationController::StopAnimation()
{
	m_globalStartTime = 0;
	m_localCurrentTime = 0;
	m_animEndTime = 0;
	m_animStartTime = 0;

	m_isAnimating = false;
	m_isPaused = false;
}

void AnimationController::ClampTime()
{
	if(m_isLooping)
	{
		if(m_localCurrentTime > 0)
		{
			long animLength = m_animEndTime - m_animStartTime;//TODO anim duration
			m_localCurrentTime = m_localCurrentTime % animLength;
		}
	}
	else
	{
		m_localCurrentTime = MathsUtils::Clamp(m_localCurrentTime, m_animStartTime, m_animEndTime);
	}
}

void AnimationController::Update(
	MeshPtr mesh,
	const long globalTime
	)
{
	BoneNode *root = mesh->GetBoneNodeHierarchy();

	float playSpeed = 1; // 2x. Slower = 1/2 times. reverse = -2x
	bool reverse = false;
	if(!reverse)
	{
		m_localCurrentTime = (globalTime - m_globalStartTime) * playSpeed;
	}
	else
	{
		m_localCurrentTime = m_animEndTime - (globalTime - m_globalStartTime) * playSpeed;
	}

	AnimationInfoPtr animationInfo = mesh->GetAnimationInfo();

	ClampTime();

	int sample = animationInfo->ConvertMillisecondsToFrame(m_localCurrentTime);

	assert(sample <= animationInfo->GetNumFrameSamples()); // Number of frames = number of frame samples - 1

	const FbxAMatrix parentGlobalScaleMatrix;// These will be identity for the root node
	const FbxAMatrix parentGlobalRotationMatrix;

	// Passing the scale and rotation matrices to avoid have to extract the rotation and scale from the parent global matrix which is non trivial (necessary to switch between scale inheritance types).
	PrepareBoneHierarcy(sample, root, parentGlobalScaleMatrix, parentGlobalRotationMatrix);
}

void AnimationController::PrepareBoneHierarcy(
	int sample,
	BoneNode *boneNode,
	const FbxAMatrix &parentGlobalScaleMatrix,
	const FbxAMatrix &parentGlobalRotationMatrix
	)
{
	for(boneNode; boneNode != NULL; boneNode = boneNode->m_next)
	{
		VectorKey localPositionKey;
		VectorKey localScaleKey;
		QuaternionKey localRotationKey;

		InterpolatePosition(sample, boneNode->GetPositionTrack(), localPositionKey);
		InterpolateRotation(sample, boneNode->GetRotationTrack(), localRotationKey);
		InterpolateScale(sample, boneNode->GetScaleTrack(), localScaleKey);

		FbxAMatrix localScaleMatrix;
		localScaleMatrix.SetS(localScaleKey.m_vector);
		FbxAMatrix localRotationMatrix;
		localRotationMatrix.SetQ(localRotationKey.m_quaternion);

		// Get the transform which needs setting
		FbxAMatrix& globalTransform = boneNode->GetGlobalTransform();

		if(boneNode->m_parent)
		{			
			const FbxAMatrix& parentTransform = boneNode->m_parent->GetGlobalTransform();

			FbxAMatrix globalRotationAndScale;

			// Bones exported from Max or Maya can inherit or not inherit scale on each node. In order to deal with this the rotation and scale must be calucated separately from the translation.

			if(boneNode->InheritsScale()) //FbxTransform::eInheritRrSs - translation * (parentRotate * localRotate * parentScale * local Scale)
			{
				globalRotationAndScale = parentGlobalRotationMatrix * localRotationMatrix * parentGlobalScaleMatrix * localScaleMatrix;
			}
			else //FbxTransform::eInheritRrs - translation * (parentRotate * localRotate * local Scale)
			{
				globalRotationAndScale = parentGlobalRotationMatrix * localRotationMatrix * localScaleMatrix;
			}

			// Transform the position by the whole parent transform so that the position gets rotated and scaled correctly.
			// Use a vector transform here as we don't want any other info from the global transform at this point.
			FbxVector4 globalPosition = parentTransform.MultT(localPositionKey.m_vector);

			FbxAMatrix globalPositionMatrix;
			globalPositionMatrix.SetT(globalPosition);
			globalTransform = globalPositionMatrix * globalRotationAndScale;

			if(boneNode->m_firstChild)
			{
				// Record the global scale and rotation matrices for use by the child
				FbxAMatrix globalScaleMatrix = parentGlobalScaleMatrix * localScaleMatrix;

				// Record the global scale and rotation matrices for use by the child
				FbxAMatrix globalRotationMatrix = parentGlobalRotationMatrix * localRotationMatrix;

				PrepareBoneHierarcy(sample, boneNode->m_firstChild, globalScaleMatrix, globalRotationMatrix); // Passing the parent scale and rotation avoids extracting them from the global transform which is non trivial
			}

		}
		else // No parent, so the global transform must be the same as the local transform
		{
			FbxAMatrix localTranslationMatrix;
			localTranslationMatrix.SetT(localPositionKey.m_vector);

			globalTransform = localTranslationMatrix * localRotationMatrix * localScaleMatrix;

			if(boneNode->m_firstChild)
			{
				PrepareBoneHierarcy(sample, boneNode->m_firstChild, localScaleMatrix, localRotationMatrix); // Passing the parent scale and rotation avoids extracting them from the global transform which is non trivial
			}

		}
	}

}

void AnimationController::InterpolatePosition(
	int sample,
	boost::shared_ptr<VectorTrack> positionTrack,
	VectorKey &result
	)
{
	const VectorKey &lastPositionKey = positionTrack->GetKey(sample);
	if(lastPositionKey.m_time == m_localCurrentTime) // First check if the time is exactly on the key
	{
		result = lastPositionKey;
	}
	else // Otherwise interpolate
	{
		const VectorKey &nextPositionKey = positionTrack->GetKey(sample + 1);

		// Find the current time value as a 0 - 1 proporion between the two keys
		const float normalizedTime = MathsUtils::NormalizeValue(
			m_localCurrentTime, lastPositionKey.m_time, nextPositionKey.m_time);
		Lerp(normalizedTime, lastPositionKey, nextPositionKey, result);
	}
}

void AnimationController::InterpolateRotation(
	int sample,
	boost::shared_ptr<QuaternionTrack> rotationTrack,
	QuaternionKey &result
	)
{
	const QuaternionKey &lastRotationKey = rotationTrack->GetKey(sample);
	if(lastRotationKey.m_time == m_localCurrentTime) // First check if the time is exactly on the key
	{
		result = lastRotationKey;
	}
	else
	{
		const QuaternionKey &nextRotationKey = rotationTrack->GetKey(sample + 1);

		// Find the current time value as a 0 - 1 proportion between the two keys
		const float normalizedTime = MathsUtils::NormalizeValue(m_localCurrentTime, lastRotationKey.m_time, nextRotationKey.m_time);
		Slerp(normalizedTime, lastRotationKey, nextRotationKey, result);
	}
}

void AnimationController::InterpolateScale(
	int sample,
	boost::shared_ptr<VectorTrack> scaleTrack,
	VectorKey &result
	)
{
	const VectorKey &lastScaleKey = scaleTrack->GetKey(sample);
	if(lastScaleKey.m_time == m_localCurrentTime) // First check if the time is exactly on the key
	{
		result = lastScaleKey;
	}
	else
	{
		const VectorKey &nextScaleKey = scaleTrack->GetKey(sample + 1);
		// Find the current time value as a 0 - 1 proportion between the two keys
		const float normalizedTime = MathsUtils::NormalizeValue(m_localCurrentTime, lastScaleKey.m_time, nextScaleKey.m_time);
		Lerp(normalizedTime, lastScaleKey, nextScaleKey, result);
	}
}

void AnimationController::Lerp(
	const float normalizedTime, // time value between 0 and 1 for interpolating betwen the keys
	const VectorKey &key,
	const VectorKey &nextKey,
	VectorKey &result
	)
{
	// keyA + (keyB - keyA) * t
	result.m_vector = key.m_vector + (nextKey.m_vector - key.m_vector) * normalizedTime; //TODO overload operator
	return;
}

void AnimationController::Lerp(
	const float normalizedTime, // time value between 0 and 1 for interpolating betwen the keys
	const QuaternionKey &key,
	const QuaternionKey &nextKey,
	QuaternionKey &result
	)
{
	// keyA + (keyB - keyA) * t
	result.m_quaternion = key.m_quaternion + (nextKey.m_quaternion - key.m_quaternion) * normalizedTime;
	return;
}

// Interpolate a quaternion rotation using SLERP
// TODO optimise ie http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
void AnimationController::Slerp(
	const float normalizedTime, // time value between 0 and 1 for interpolating betwen the keys
	const QuaternionKey &key,
	const QuaternionKey &nextKey,
	QuaternionKey &result
	)
{
	FbxQuaternion nextQuaternion = nextKey.m_quaternion; //Copy this incase we need to negate it

	// First calcualte the 4D dot product which gives cos theta
	double cosTheta = key.m_quaternion[0] * nextQuaternion[0] + key.m_quaternion[1] * nextQuaternion[1]
					+ key.m_quaternion[2] * nextQuaternion[2] + key.m_quaternion[3] * nextQuaternion[3];

	if(cosTheta == 1)
	{
		result.m_quaternion = key.m_quaternion; // The two key are the same so just return.
		return;
	}

	if(cosTheta < 0) // q1 and q2 are more than 90 degrees apart so invert one of them to reduce spinning
	{
		cosTheta *= -1;
		nextQuaternion = -nextKey.m_quaternion;
	}

	if(cosTheta < 0.99999f) // If the keys are different
	{
		double theta = acos(cosTheta);
		double firstKeyWeight = sin((1 - normalizedTime) * theta) / sin(theta);
		double nextKeyWeight = sin(normalizedTime * theta) / sin(theta);

		FbxQuaternion interpolatedQuat = (key.m_quaternion * firstKeyWeight) + (nextQuaternion * nextKeyWeight);

		result = QuaternionKey(interpolatedQuat, 0);
		return;
	}

	return Lerp(normalizedTime, key, nextKey, result); // Keys are pretty much the same so just lerp
}


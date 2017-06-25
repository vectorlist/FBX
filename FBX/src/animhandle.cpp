#include <animhandle.h>
#include <node.h>
#include <fbxtool.h>
#include <log.h>

AnimHandle::AnimHandle()
	:
	mIsAnimating(false),
	mSpeed(1.0f),
	mGlobalTime(0.0f)
{

}


AnimHandle::~AnimHandle()
{

}

void AnimHandle::updateNodes(Node *node, const float delta)
{
	BoneNode* RootBoneNode = node->getBoneNodeRoot();
	AnimSample* Sample = node->getCurrentSample();

	//Increase Global time at runtime
	mGlobalTime += delta;

	//LOG << Sample->mStart << ENDN;
	//Global % Animation Length + Animation Start Time
	float SampleTime = fmod(mGlobalTime, Sample->mDuration) + Sample->mStart;

	//LOG << "Global : " << mGlobalTime << " Sample : " <<SampleTime << ENDN;
	
	const FbxAMatrix parentGlobalScale;
	const FbxAMatrix parentGlobalRotation;

	evalNodes(SampleTime, RootBoneNode, parentGlobalScale, parentGlobalRotation);
}

void AnimHandle::evalNodes(float sample,
	BoneNode * pBoneNode,
	const FbxAMatrix &parentS,
	const FbxAMatrix &parentR)
{
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->mNext)
	{
	
		//TODO : replace Track container
		TrackVec3* PositionTrack = pBoneNode->getPositionTrack();
		TrackVec3* ScaleTrack = pBoneNode->getScaleTrack();
		TrackQuaternion* RotationTrack = pBoneNode->getQuaternionTrack();

		//interpolate each
		auto Position = interpolatePosition(sample, PositionTrack);
		auto Scale = interpolateScale(sample, ScaleTrack);
		auto Roatation = interpolateRotation(sample, RotationTrack);
		
		//Matrix
		FbxAMatrix localS;
		localS.SetS(Scale);
		FbxAMatrix localR;
		localR.SetQ(Roatation.mQuaternion);

		//for record all matrix (trans * rot * scale) to this bone node
		FbxAMatrix& globalTransform = pBoneNode->getGlobalTransfrom();
		
		if (pBoneNode->mParent)
		{
			const FbxAMatrix& parentTransform = pBoneNode->mParent->getGlobalTransfrom();
			FbxAMatrix globalRS;

			if (pBoneNode->getInheritScale())
			{
				globalRS = parentR * localR * parentS * localS;
			}
			else
			{
				globalRS = parentR * localR * localS;
			}
			
			FbxVector4 globalPosion = parentTransform.MultT(Position);

			FbxAMatrix globalT;
			globalT.SetT(globalPosion);
			globalTransform = globalT * globalRS;

			if (pBoneNode->mFirstChild)
			{
				FbxAMatrix globalS = parentS * localS;
				FbxAMatrix globalR = parentR * localR;

				evalNodes(sample, pBoneNode->mFirstChild, globalS, globalR);
			}
		}
		else
		{
			FbxAMatrix localT;
			localT.SetT(Position);

			globalTransform = localT * localR * localS;
		
			if (pBoneNode->mFirstChild)
			{
				evalNodes(sample, pBoneNode->mFirstChild, localS, localR);
			}
		}
	}
}



FbxVector4 AnimHandle::interpolatePosition(float SampleTime, TrackVec3 *Track)
{
	FbxVector4 Position;
	int Frame = 0;
	for (int i = 0; i < Track->mKeyNums - 1; ++i) {
		if (SampleTime < (float)Track->getKey(i + 1).mTime)
		{
			//Current Frame;
			Frame = i;
			break;
		}
	}

	//Get Current Key and Next Key
	KeyVec3 Current = Track->getKey(Frame);
	KeyVec3 Next = Track->getKey(Frame + TRACK_INTERVAL);
	
	//Found Noramlized Time Btw Two Keys
	float NormalizedTime = VML::normalize(Current.mTime, Next.mTime, SampleTime);

	//Lerp
	Position = Lerp(Current.mVector, Next.mVector, NormalizedTime);
	return Position;
}

FbxVector4 AnimHandle::interpolateScale(float SampleTime, TrackVec3* Track)
{
	FbxVector4 Scale;
	int Frame = 0;
	for (int i = 0; i < Track->mKeyNums - 1; ++i)
	{
		if (SampleTime < (float)Track->getKey(i + 1).mTime)
		{
			Frame = i;
			break;
		}
	}
	KeyVec3 Current = Track->getKey(Frame);
	KeyVec3 Next = Track->getKey(Frame + TRACK_INTERVAL);

	float NormalizedTime = VML::normalize(Current.mTime, Next.mTime, SampleTime);

	Scale = Lerp(Current.mVector, Next.mVector, NormalizedTime);
	return Scale;
}

KeyQuaternion AnimHandle::interpolateRotation(float SampleTime, TrackQuaternion* Track)
{
	KeyQuaternion Rotation;
	int Frame = 0;
	for (int i = 0; i < Track->mKeyNums - 1; ++i)
	{
		if (SampleTime < (float)Track->getKey(i + 1).mTime)
		{
			Frame = i;
			break;
		}
	}

	KeyQuaternion Current = Track->getKey(Frame);
	KeyQuaternion Next = Track->getKey(Frame + TRACK_INTERVAL);

	float NormalizedTime = VML::normalize(Current.mTime, Next.mTime, SampleTime);

	Slerp(NormalizedTime, Current, Next, Rotation);
	return Rotation;
}

FbxVector4 AnimHandle::Lerp(const FbxVector4 &Current, const FbxVector4 &Next, float Percent)
{
	//https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
	return Current + (Next - Current) * Percent;
}

void AnimHandle::Slerp(
	const float normalizedTime, 
	const KeyQuaternion &key,
	const KeyQuaternion &nextKey,
	KeyQuaternion &result)
{
	FbxQuaternion nextQuaternion = nextKey.mQuaternion; //Copy this incase we need to negate it

														 // First calcualte the 4D dot product which gives cos theta
	double cosTheta = key.mQuaternion[0] * nextQuaternion[0] + key.mQuaternion[1] * nextQuaternion[1]
		+ key.mQuaternion[2] * nextQuaternion[2] + key.mQuaternion[3] * nextQuaternion[3];

	if (cosTheta == 1)
	{
		result.mQuaternion = key.mQuaternion; // The two key are the same so just return.
		return;
	}

	if (cosTheta < 0) // q1 and q2 are more than 90 degrees apart so invert one of them to reduce spinning
	{
		cosTheta *= -1;
		nextQuaternion = -nextKey.mQuaternion;
	}

	if (cosTheta < 0.99999f) // If the keys are different
	{
		double theta = acos(cosTheta);
		double firstKeyWeight = sin((1 - normalizedTime) * theta) / sin(theta);
		double nextKeyWeight = sin(normalizedTime * theta) / sin(theta);

		FbxQuaternion interpolatedQuat = (key.mQuaternion * firstKeyWeight) + (nextQuaternion * nextKeyWeight);

		result = KeyQuaternion(interpolatedQuat, 0);
		return;
	}

	return Lerp(normalizedTime, key, nextKey, result); 
}

void AnimHandle::Lerp(
	float NormalizeTime,
	const KeyQuaternion &Current,
	const KeyQuaternion &Next,
	KeyQuaternion &Result)
{
	Result.mQuaternion = Current.mQuaternion + (Next.mQuaternion - Current.mQuaternion) * NormalizeTime;
}
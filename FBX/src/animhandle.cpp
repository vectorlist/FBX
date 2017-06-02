#include <animhandle.h>
#include <node.h>
#include <fbxtool.h>
#include <log.h>

AnimHandle::AnimHandle()
	: mGlobalStartTime(0),
	mLocalCurrentTime(0),
	mAnimStartTime(0),
	mAnimEndTime(0),
	mLocalFrame(0),
	mGlobalFrame(0),
	mIsLoop(true),
	mIsAnimating(false),
	mIsPause(false)
{

}


AnimHandle::~AnimHandle()
{

}

//replace to ref to Sample
bool AnimHandle::startAnimation(
	long globalStartTime,
	long animationStartTime,
	long animationEndTime)
{
	if (animationEndTime == 0) {
		return 0;
	}

	mGlobalStartTime = globalStartTime;

	mAnimStartTime = animationStartTime;
	mAnimEndTime = animationEndTime;

	mIsAnimating = true;
	mIsPause = false;

	return true;
}

bool AnimHandle::isAnimating() const
{
	return mIsAnimating;
}

bool AnimHandle::isPause() const
{
	return mIsPause;
}

void AnimHandle::setLoop(bool loop)
{
	mIsLoop = loop;
}

void AnimHandle::updateNodes(Node *node, const long globalTime)
{
	BoneNode* rootBoneNode = node->getBoneNodeRoot();

	mLocalCurrentTime = (globalTime - mGlobalStartTime);

	AnimSample* sample = node->getCurrentSample();

	clampLocalCurrentTime();

	int sampleFrame = sample->convertMilliToFrame(mLocalCurrentTime) + sample->getSampleBlock();
	mLocalFrame = sampleFrame;
	mGlobalFrame += mLocalFrame;
	//LOG << sampleFrame << ENDN;

	const FbxAMatrix parentGlobalScale;
	const FbxAMatrix parentGlobalRotation;

	evalNodes(sampleFrame, rootBoneNode, parentGlobalScale, parentGlobalRotation);
}

void AnimHandle::evalNodes(int sample,
	BoneNode * pBoneNode,
	const FbxAMatrix &parentS,
	const FbxAMatrix &parentR)
{
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->mNext)
	{
		KeyVec3 positionKey;
		KeyVec3 scaleKey;
		KeyQuaternion rotationKey;

		//TODO : replace Track container
		TrackVec3* posTrack = pBoneNode->getPositionTrack();
		TrackVec3* scaleTrack = pBoneNode->getScaleTrack();
		TrackQuaternion* rotTrack = pBoneNode->getQuaternionTrack();

		//interpolate each
		interpolatePosition(sample, posTrack, positionKey);
		interpolateScale(sample, scaleTrack, scaleKey);
		interpolateRoation(sample, rotTrack, rotationKey);

		//Matrix
		FbxAMatrix localS;
		localS.SetS(scaleKey.mVector);
		FbxAMatrix localR;
		localR.SetQ(rotationKey.mQuaternion);

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
			
			FbxVector4 globalPosion = parentTransform.MultT(positionKey.mVector);

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
			localT.SetT(positionKey.mVector);

			globalTransform = localT * localR * localS;
		
			if (pBoneNode->mFirstChild)
			{
				evalNodes(sample, pBoneNode->mFirstChild, localS, localR);
			}
		}
	}
}

void AnimHandle::clampLocalCurrentTime()
{
	if (mIsLoop)
	{
		if (mLocalCurrentTime > 0)
		{
			long animationLength = mAnimEndTime - mAnimStartTime;
			mLocalCurrentTime = mLocalCurrentTime % animationLength;
		}
	}
	else
	{
		mLocalCurrentTime = FBXTool::clamp(mLocalCurrentTime, mAnimStartTime, mAnimEndTime);
	}
}

void AnimHandle::interpolatePosition(int sample, TrackVec3 * positionTrack, KeyVec3 &output)
{
	const KeyVec3 &lastKey = positionTrack->getKey(sample);
	if (lastKey.mTime == mLocalCurrentTime)
	{
		output = lastKey;
		return;
	}
	const KeyVec3 &nextKey = positionTrack->getKey(sample + 1);
	const float normalizedSample = normalize(lastKey.mTime, nextKey.mTime, mLocalCurrentTime);
	FBXTool::lerp(lastKey, nextKey, output, normalizedSample);
}

void AnimHandle::interpolateScale(int sample, TrackVec3* scaleTrack, KeyVec3 &output)
{
	const KeyVec3 &lastKey = scaleTrack->getKey(sample);
	if (lastKey.mTime == mLocalCurrentTime) {
		output = lastKey;
		return;
	}
	const KeyVec3 &nextKey = scaleTrack->getKey(sample + 1);
	const float normalizedSample = normalize(lastKey.mTime, nextKey.mTime, mLocalCurrentTime);
	FBXTool::lerp(lastKey, nextKey, output,normalizedSample);
}

void AnimHandle::interpolateRoation(int sample, TrackQuaternion* rotationTrack, KeyQuaternion& output)
{
	const KeyQuaternion &lastKey = rotationTrack->getKey(sample);
	if (lastKey.mTime == mLocalCurrentTime) {
		output = lastKey;
		return;
	}
	const KeyQuaternion &next = rotationTrack->getKey(sample + 1);
	const float normalizedSaple = normalize(lastKey.mTime, next.mTime, mLocalCurrentTime);
	FBXTool::slerp(lastKey, next, output, normalizedSaple);
}

float AnimHandle::normalize(float last, float next, float current)
{
	float normalizedCurrent = (current - last) / (float)(next - last);
	return normalizedCurrent;
}

long AnimHandle::getLocalTime() const
{
	return mLocalCurrentTime;
}

int AnimHandle::getLocalFrame() const
{
	return mLocalFrame;
}

int AnimHandle::getGlobalFrame() const
{
	return mGlobalFrame;
}

void AnimHandle::debugSampleKey(const KeyVec3 & last, const KeyVec3 & next)
{
	LOG << "last key : " << last.mTime << " next key : " << next.mTime <<
		" local time : " << mLocalCurrentTime << ENDN;
}


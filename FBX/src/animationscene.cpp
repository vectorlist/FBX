#include <animationscene.h>
#include <node.h>
#include <fbxtool.h>
#include <log.h>

AnimationScene::AnimationScene()
	: mGlobalStartTime(0),
	mLocalCurrentTime(0),
	mAnimStartTime(0),
	mAnimEndTime(0),
	mIsLoop(true),
	mIsAnimating(false),
	mIsPause(false)
{

}


AnimationScene::~AnimationScene()
{

}

//replace to ref to Sample
bool AnimationScene::startAnimation(
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

bool AnimationScene::isAnimating() const
{
	return mIsAnimating;
}

bool AnimationScene::isPause() const
{
	return mIsPause;
}

void AnimationScene::setLoop(bool loop)
{
	mIsLoop = loop;
}

//void AnimationScene::updateNode(Node *node, const long globalTime)
//{
//	//input golbalTime must millisecond
//
//	//TASK 0 : get Bone Node(root)
//	BoneNode* rootBoneNode = node->getBoneNodeRoot();
//
//	//need to fix
//	mLocalCurrentTime = (globalTime - mGlobalStartTime);
//	
//	//TASK 1 : get animation info from node
//	AnimationSample* sample = node->getAnimationSample();
//
//	//TASK 2 : set to clamp local current time
//	clampLocalCurrentTime();
//
//	//TASK 3 : conversasion animation samples
//	int sampleFrame = sample->convertMilliToFrame(mLocalCurrentTime);
//	
//	if (sampleFrame > sample->getSamplesFrameNum()) {
//		LOG_ASSERT("out of samples per frame");
//	}
//	LOG << sampleFrame << ENDN;
//	//TASK 4 : get Child Bone Node and calulate
//	const FbxAMatrix parentGlobalScale;
//	const FbxAMatrix parentGlobalRotation;
//
//	evalNodes(sampleFrame, rootBoneNode, parentGlobalScale, parentGlobalRotation);
//	//evalFrame(sampleFrame, rootBoneNode);
//}

void AnimationScene::updateNode(Node *node, const long globalTime)
{
	//input golbalTime must millisecond

	//TASK 0 : get Bone Node(root)
	BoneNode* rootBoneNode = node->getBoneNodeRoot();

	//TODO :: add offsets
	mLocalCurrentTime = (globalTime - mGlobalStartTime);

	//TASK 1 : get animation info from node
	AnimationSample* sample = node->currentSample;

	//TASK 2 : set to clamp local current time
	clampLocalCurrentTime();

	int offsetSample = sample->getSampleOffset();
	//LOG << offsetSample << ENDN;
	//TASK 3 : conversasion animation samples
	int sampleFrame = sample->convertMilliToFrame(mLocalCurrentTime) + offsetSample;

	/*if (sampleFrame > sample->getSamplesFrameNum()) {
		LOG_ASSERT("out of samples per frame");
	}*/
	//LOG << sampleFrame << ENDN;
	//TASK 4 : get Child Bone Node and calulate
	const FbxAMatrix parentGlobalScale;
	const FbxAMatrix parentGlobalRotation;

	evalNodes(sampleFrame, rootBoneNode, parentGlobalScale, parentGlobalRotation);
	//evalFrame(sampleFrame, rootBoneNode);
}

void AnimationScene::evalNodes(int sample,
	BoneNode * pBoneNode,
	const FbxAMatrix &parentS,
	const FbxAMatrix &parentR)
{
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->next())
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
		
		if (pBoneNode->parent())
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

			FbxAMatrix globalPositionMatrix;
			globalPositionMatrix.SetT(globalPosion);
			globalTransform = globalPositionMatrix * globalRS;

			if (pBoneNode->child())
			{
				FbxAMatrix globalS = parentS * localS;
				FbxAMatrix globalR = parentR * localR;

				evalNodes(sample, pBoneNode->child(), globalS, globalR);
			}
		}
		else
		{
			FbxAMatrix localT;
			localT.SetT(positionKey.mVector);

			globalTransform = localT * localR * localS;
		
			if (pBoneNode->child())
			{
				//found child must recursive


				evalNodes(sample, pBoneNode->child(), localS, localR);
			}
		}
	}
}

//test
void AnimationScene::evalFrame(int sample, BoneNode* pNode)
{
	for (BoneNode* node = pNode; node != NULL; node = node->mNext)
	{
		auto* posTrack = node->getPositionTrack();
		auto* rotTrack = node->getQuaternionTrack();
		auto* scaleTrack = node->getScaleTrack();

		auto& posKey = posTrack->getKey(sample);
		auto& rotKey = rotTrack->getKey(sample);
		auto& scaleKey = scaleTrack->getKey(sample);

		//FbxAMatrix localpos;
		//localpos.
		FbxAMatrix T, R, S;
		T.SetT(posKey.mVector);
		R.SetQ(rotKey.mQuaternion);
		S.SetS(scaleKey.mVector);
		node->mGlobalTransform = T * R * S;

		if (node->mEndChild)
		{
			evalFrame(sample, node->mEndChild);
		}
	}
}

void AnimationScene::interpolatePosition(int sample, TrackVec3 * positionTrack, KeyVec3 &output)
{
	const KeyVec3 &lastKey = positionTrack->getKey(sample);
	if (lastKey.mTime == mLocalCurrentTime)
	{
		output = lastKey;
		return;
	}
	//frame sample dosent matched has Lerp Animation
	const KeyVec3 &nextKey = positionTrack->getKey(sample + 1);
	//TODO : lerp animation (normalize)
	float normalizedSample = normalize(lastKey.mTime, nextKey.mTime, mLocalCurrentTime);
	FBXTool::lerp(lastKey, nextKey, output, normalizedSample);
}

void AnimationScene::interpolateScale(int sample, TrackVec3* scaleTrack, KeyVec3 &output)
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

void AnimationScene::interpolateRoation(int sample, TrackQuaternion* rotationTrack, KeyQuaternion& output)
{
	const KeyQuaternion &lastKey = rotationTrack->getKey(sample);
	if (lastKey.mTime == mLocalCurrentTime) {
		output = lastKey;
		return;
	}
	const KeyQuaternion &next = rotationTrack->getKey(sample + 1);
	float normalizedSaple = normalize(lastKey.mTime, next.mTime, mLocalCurrentTime);
	FBXTool::slerp(lastKey, next, output, normalizedSaple);
}

float AnimationScene::normalize(float last, float next, float current)
{
	float normalizedCurrent = (current - last) / (float)(next - last);
	return normalizedCurrent;
}

void AnimationScene::debugSampleKey(const KeyVec3 & last, const KeyVec3 & next)
{
	LOG << "last key : " << last.mTime << " next key : " << next.mTime <<
		" local time : " << mLocalCurrentTime << ENDN;
}


void AnimationScene::clampLocalCurrentTime()
{
	//on looping
	if (mIsLoop)
	{
		if (mLocalCurrentTime > 0)
		{
			long animationLength = mAnimEndTime - mAnimStartTime;
			mLocalCurrentTime = mLocalCurrentTime % animationLength;
		}
	}
	//pause or etc
	else
	{
		mLocalCurrentTime = FBXTool::clamp(mLocalCurrentTime, mAnimStartTime, mAnimEndTime);
	}
}


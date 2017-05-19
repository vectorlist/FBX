#include <animationscene.h>
#include <node.h>
#include <fbxtool.h>
#include <log.h>
#include <logger.h>

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

void AnimationScene::updateNode(Node *node, const long globalTime)
{
	//input golbalTime must millisecond

	//TASK 0 : get Bone Node(root)
	BoneNode* rootBoneNode = node->getBoneNodeRoot();

	//need to fix
	mLocalCurrentTime = (globalTime - mGlobalStartTime);
	
	//TASK 1 : get animation info from node
	AnimationSample* sample = node->getAnimationSample();

	//TASK 2 : set to clamp local current time
	clampLocalCurrentTime();

	//TASK 3 : conversasion animation samples
	int sampleFrame = sample->convertMilliToFrame(mLocalCurrentTime);
	
	if (sampleFrame > sample->getSamplesFrameNum()) {
		LOG_ASSERT("out of samples per frame");
	}

	//TASK 4 : get Child Bone Node and calulate
	boneRecursive(sampleFrame, rootBoneNode);
}

void AnimationScene::boneRecursive(int sample, BoneNode *pBoneNode)
{
	//TASK 0 : FOR EACH CHILD BONE NODE
	//BoneNode* node = pBoneNode;
	//TODO : fix NodeTree
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->mNext)
	{
		//LOG << "founded node : " << pBoneNode->getName() << ENDN;
		//TASK 0 : get all key at this frame
		KeyVec3 positionKey;
		KeyVec3 scaleKey;
		KeyQuaternion rotationKey;

		//TODO : replace Track container
		//get track from bone node
		TrackVec3* posTrack = pBoneNode->getPositionTrack();
		TrackVec3* scaleTrack = pBoneNode->getScaleTrack();
		TrackQuaternion* rotTrack = pBoneNode->getQuaternionTrack();

		//interpolate each
		interpolatePosition(sample, posTrack, positionKey);
		interpolateScale(sample, scaleTrack, scaleKey);
		interpolateRoation(sample, rotTrack, rotationKey);

		//Matrix
		FbxAMatrix localScale;
		localScale.SetS(scaleKey.mVector);
		FbxAMatrix localRoatation;
		localRoatation.SetQ(rotationKey.mQuaternion);
		//LOG_FBX_MATRIX(localRoatation);
		//LOG << rotationKey << ENDN;

		//for record all matrix (trans * rot * scale) to this bone node
		FbxAMatrix& globalTransform = pBoneNode->getGlobalTransfrom();

		//LOG_FBX_MATRIX(localRoatation);
		//LOG << rotationKey << ENDN;
		if (pBoneNode->mParent)
		{
			//TODO : if found parent record all transform
			LOG << "found parent" << ENDN;
		}
		else
		{
			FbxAMatrix localTransform;
			localTransform.SetT(positionKey.mVector);

			//record matrix to node
			globalTransform = localTransform * localRoatation * localScale;
			/*LOG << "bone name : " << pBoneNode->getName() << ENDN;*/
			//LOG_FBX_MATRIX(globalTransform);

			if (pBoneNode->mFirstChild)
			{
				//found child must recursive
				

				boneRecursive(sample, pBoneNode->mFirstChild);
			}
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
	float normalizedSample = normalize(lastKey.mTime, nextKey.mTime, mLocalCurrentTime);
	FBXTool::lerp(lastKey, nextKey, normalizedSample);
}

void AnimationScene::interpolateRoation(int sample, TrackQuaternion* rotationTrack, KeyQuaternion& output)
{
	const KeyQuaternion last = rotationTrack->getKey(sample);
	if (last.mTime == mLocalCurrentTime) {
		output = last;
		return;
	}
	const KeyQuaternion next = rotationTrack->getKey(sample + 1);
	float normalizedSaple = normalize(last.mTime, next.mTime, mLocalCurrentTime);
	FBXTool::slerp(last, next, output, normalizedSaple);
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


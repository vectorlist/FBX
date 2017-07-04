#include <config.h>
#include <animhandle.h>
#include <node.h>
#include <log.h>
#include <animlayer.h>

AnimHandle::AnimHandle()
	:
	mIsAnimating(false),
	mSpeed(1.0f),
	mGlobalTime(0.0f),
	mLocalKeyFrame(0)
{

	mSpeed = 1.f;
}


AnimHandle::~AnimHandle()
{

}

void AnimHandle::UpdateNodes(Node *node, const float delta)
{
	BoneNode* root = node->GetBoneNodeRoot();
	AnimSample* sample = node->GetCurrentSample();

	mGlobalTime += delta * mSpeed;
	
	//Global % Animation Length + Animation Start Time
	float sampleTime = fmod(mGlobalTime, sample->mDuration) + sample->mStart;

	int sampleFrame = GetLocalFrame(sampleTime, root->GetPositionTrack());

	const Matrix4x4 ParentS;
	const Matrix4x4 ParentR;

	AnimSample* nextSample = node->GetNextSample();
	if (!nextSample) {
		mWeight = 0.0f;
	}
	if (nextSample && sample != nextSample) 
	{
		//LOG << "walk" << ENDN;
		float nextSampleTime = GetLocalTime(mGlobalTime, nextSample);
		int nextSampleFrame = GetLocalFrame(nextSampleTime, root->GetPositionTrack());
		if (mWeight != 1.0f) {
			mWeight = GetLocalFactor(nextSampleFrame, nextSample);
		}
		
		//LOG << "c : " << sampleFrame <<" n : " <<nextSampleFrame << ENDN;
		//mWeight = GetLocalFactor(nextSampleFrame, nextSample);
		const Matrix4x4 R;
		const Matrix4x4 S;
		//EvalNextNodes(nextSampleTime, nextSampleFrame, root, S, R);
		EvalNextNodes(nextSampleTime, nextSampleFrame, root, S, R);
	}
	/*else {
		EvalNodes(sampleTime, sampleFrame, root, ParentS, ParentR);
	}*/
	/*EvalBlendNodes(SampleTime, mLocalKeyFrame, Root, parentS, parentS);*/
	EvalNodes(sampleTime, sampleFrame, root, ParentS, ParentR);
}

void AnimHandle::EvalNodes(
	float sample,
	int frame,
	BoneNode * pBoneNode,
	const Matrix4x4 &ParentS,
	const Matrix4x4 &ParentR)
{
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->mNext)
	{
		VectorTrack* PositionTrack = pBoneNode->GetPositionTrack();
		VectorTrack* ScaleTrack = pBoneNode->GetScaleTrack();
		QautTrack* RotationTrack = pBoneNode->GetRotationTrack();

		auto Position	= InterpolatePosition(sample, frame,PositionTrack);
		auto Scale		= InterpolateScale(sample, frame,ScaleTrack);
		auto Roatation	= InterpolateRotation(sample, frame,RotationTrack);
		
		Matrix4x4 LocalS;
		LocalS.scale(Scale);
		Matrix4x4 LocalR = Roatation.GetMatrix();
		
		Matrix4x4& GlobalTransform = pBoneNode->GetGlobalTransform();
		//LOG << GlobalTransform << ENDN;
		if (pBoneNode->mParent)
		{
			const Matrix4x4& ParentTransform = pBoneNode->mParent->GetGlobalTransform();
			Matrix4x4 GlobalRS;

			if (pBoneNode->GetInheritScale())
			{
				GlobalRS = ParentR * LocalR * ParentS * LocalS;
			}
			else
			{
				GlobalRS = ParentR * LocalR * LocalS;
			}
			
			vec3f GlobalPosion = ParentTransform * Position;

			Matrix4x4 GlobalT;
			GlobalT.translate(GlobalPosion);
			GlobalTransform = GlobalT * GlobalRS;

			if (pBoneNode->mFirstChild)
			{
				Matrix4x4 GlobalS = ParentS * LocalS;
				Matrix4x4 GlobalR = ParentR * LocalR;

				EvalNodes(sample, frame, pBoneNode->mFirstChild, GlobalS, GlobalR);
			}
		}
		else
		{
			Matrix4x4 LocalT;
			LocalT.translate(Position);
			GlobalTransform = LocalT * LocalR * LocalS;
			
			if (pBoneNode->mFirstChild)
			{
				EvalNodes(sample, frame, pBoneNode->mFirstChild, LocalS, LocalR);
			}
		}
	}
}

void AnimHandle::EvalNextNodes(
	float sample,
	int frame,
	BoneNode * pBoneNode,
	const Matrix4x4 &ParentS,
	const Matrix4x4 &ParentR)
{
	for (pBoneNode; pBoneNode != NULL; pBoneNode = pBoneNode->mNext)
	{
		VectorTrack* PositionTrack = pBoneNode->GetPositionTrack();
		VectorTrack* ScaleTrack = pBoneNode->GetScaleTrack();
		QautTrack* RotationTrack = pBoneNode->GetRotationTrack();

		auto Position = InterpolatePosition(sample, frame, PositionTrack);
		auto Scale = InterpolateScale(sample, frame, ScaleTrack);
		auto Roatation = InterpolateRotation(sample, frame, RotationTrack);

		Matrix4x4 LocalS;
		LocalS.scale(Scale);
		Matrix4x4 LocalR = Roatation.GetMatrix();
		
		//For Next Sample (UBO)
		Matrix4x4& nextTransform = pBoneNode->GetNextGlobalTransform();

		if (pBoneNode->mParent)
		{
			const Matrix4x4& ParentTransform = pBoneNode->mParent->GetNextGlobalTransform();
			Matrix4x4 GlobalRS;

			if (pBoneNode->GetInheritScale())
			{
				GlobalRS = ParentR * LocalR * ParentS * LocalS;
			}
			else
			{
				GlobalRS = ParentR * LocalR * LocalS;
			}

			vec3f GlobalPosion = ParentTransform * Position;

			Matrix4x4 GlobalT;
			GlobalT.translate(GlobalPosion);
			nextTransform = GlobalT * GlobalRS;

			if (pBoneNode->mFirstChild)
			{
				Matrix4x4 GlobalS = ParentS * LocalS;
				Matrix4x4 GlobalR = ParentR * LocalR;

				EvalNextNodes(sample, frame, pBoneNode->mFirstChild, GlobalS, GlobalR);
			}
		}
		else
		{
			Matrix4x4 LocalT;
			LocalT.translate(Position);
			nextTransform = LocalT * LocalR * LocalS;

			if (pBoneNode->mFirstChild)
			{
				EvalNextNodes(sample, frame, pBoneNode->mFirstChild, LocalS, LocalR);
			}
		}
	}
}

vec3f AnimHandle::InterpolatePosition(float sample, int frame,VectorTrack *track)
{
	vec3f position;
	const VectorKey& current = track->GetKey(frame);
	const VectorKey& next = track->GetKey(frame + TRACK_INTERVAL);
	
	float factor = vml::normalize(current.mTime, next.mTime, sample);

	position = vec3f::lerp(current.mVector, next.mVector, factor);
	return position;
}

vec3f AnimHandle::InterpolateScale(float sample, int frame,VectorTrack* track)
{
	vec3f scale;
	const VectorKey& current = track->GetKey(frame);
	const VectorKey& next = track->GetKey(frame + TRACK_INTERVAL);

	float factor = vml::normalize(current.mTime, next.mTime, sample);

	scale = vec3f::lerp(current.mVector, next.mVector, factor);
	return scale;
}

Quaternion AnimHandle::InterpolateRotation(float sample, int frame,QautTrack* track)
{
	Quaternion rotation;
	const QuatKey &current = track->GetKey(frame);
	const QuatKey &next = track->GetKey(frame + TRACK_INTERVAL);

	float factor = vml::normalize(current.mTime, next.mTime, sample);

	Quaternion::Interpolate(rotation, current.mQaut, next.mQaut, factor);
	rotation.Noramlize();
	return rotation;
}

float AnimHandle::GetLocalTime(float globalTime, AnimSample * sample)
{
	return fmod(globalTime, sample->mDuration) + sample->mStart;
}

int AnimHandle::GetLocalFrame(float sample, VectorTrack *Track)
{
	int frame = 0;
	for (int i = 0; i < Track->mKeyNums - 1; ++i) {
		if (sample < Track->GetKey(i + 1).mTime)
		{
			frame = i;
			break;
		}
	}
	return frame;
}

float AnimHandle::GetLocalFactor(int frame, AnimSample *Sample)
{
	float frameStart = static_cast<float>(frame);
	float frameEnd = static_cast<float>(Sample->mFrameStart + Sample->mFrameCount -3);
	float result = vml::normalize(0.0f, frameEnd, frameStart);
	return result;
}


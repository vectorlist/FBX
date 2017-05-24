#include "BoneNode.h"
#include "QuaternionTrack.h"
#include "VectorTrack.h"
#include "VectorKey.h"
#include "QuaternionKey.h"


unsigned int BoneNode::m_counter = 0;

BoneNode::BoneNode()
	: m_inheritScale(false),
	m_rotationTrack(NULL),
	m_positionTrack(NULL),
	m_scaleTrack(NULL),
	m_id(m_counter++) //TODO could do with something less error prone than an int id - name crc maybe?
{
};

// Allocate memory for an animation track. Currently only one animation track is supported.
void BoneNode::AllocateAnimationTracks(
	int numFrames
	)
{
	m_rotationTrack = boost::shared_ptr<QuaternionTrack>(new QuaternionTrack(numFrames));
	m_scaleTrack = boost::shared_ptr<VectorTrack>(new VectorTrack(numFrames));
	m_positionTrack = boost::shared_ptr<VectorTrack>(new VectorTrack(numFrames));
}

void BoneNode::AddPositionKey(
	const VectorKey &position
	)
{
	m_positionTrack->AddKey(position);
};

void BoneNode::AddRotationKey(
	const QuaternionKey &rotation
	)
{
	m_rotationTrack->AddKey(rotation);
};

void BoneNode::AddScaleKey(
	const VectorKey &scale
	)
{
	m_scaleTrack->AddKey(scale);
};

BoneNode::~BoneNode()
{
};



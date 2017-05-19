#pragma once

#include <track.h>
#include <vector>

typedef std::vector<TrackContainer> TrackContainerArray;

class TrackContainer
{
public:

	void allocate(int frameLength);

	TrackVec3* getPositionTrack();
	TrackVec3* getScaleTrack();
	TrackQuaternion* getRotationTrack();

private:
	TrackVec3Ptr posisition;
	TrackVec3Ptr scale;
	TrackQuaternionPtr rotation;
};

inline void TrackContainer::allocate(int frameLength)
{
	posisition = TrackVec3Ptr(new TrackVec3(frameLength));
	scale = TrackVec3Ptr(new TrackVec3(frameLength));
	rotation = TrackQuaternionPtr(new TrackVec3(frameLength));
}

inline TrackVec3* TrackContainer::getPositionTrack()
{
	return posisition.get();
}

inline TrackVec3* TrackContainer::getScaleTrack()
{
	return scale.get();
}

inline TrackQuaternion* TrackContainer::getRotationTrack()
{
	return rotation.get();
}

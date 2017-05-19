#pragma once

#include <track.h>

class TrackContainer
{
public:

	void allocate(int frameLength);


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
#pragma once

#include "Track.h"
#include "VectorKey.h"


class VectorTrack : public Track
{
public:

	VectorTrack(
		const int totalNumKeys
		) : Track()
	{
		m_keys.reserve(totalNumKeys);

	}

	void VectorTrack::AddKey(
		const VectorKey &key
		)
	{
		m_keys.push_back(key);
	}

	const VectorKey &VectorTrack::GetKey(
		int key
		)
	{
		return m_keys[key];
	}


	private:
		VectorKeyArray m_keys;

};


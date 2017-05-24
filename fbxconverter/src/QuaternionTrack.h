#pragma once

#include "Track.h"
#include "QuaternionKey.h"

class QuaternionTrack : public Track
{
public:

	QuaternionTrack(
		const int totalNumKeys
		) : Track()
	{
		m_keys.reserve(totalNumKeys);

	}

	void QuaternionTrack::AddKey(
		const QuaternionKey &key
		)
	{
		m_keys.push_back(key);
	}

	const QuaternionKey& QuaternionTrack::GetKey(
		int key
		)
	{
		return m_keys[key];
	}


	private:
		QuaternionKeyArray m_keys;

};

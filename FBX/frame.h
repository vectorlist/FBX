#pragma once

#include <cstdlib>

class Frame
{
public:
	Frame() : mCurrentFrame(0), mNumFrames(0) {}
	virtual~Frame() {}
	int mCurrentFrame;
	int mNumFrames;

	virtual void setNumFrames(int numFrames);
	virtual int getNumFrames();
};

inline void Frame::setNumFrames(int numFrames)
{
	mNumFrames = numFrames;
}

inline int Frame::getNumFrames()
{
	return mNumFrames;
}

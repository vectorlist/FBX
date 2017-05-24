#pragma once

#include <string>
#include "MathsUtils.h"


class AnimationInfo
{
public :

	AnimationInfo();
	~AnimationInfo(){};

	void SetFPS(float fps)
	{
		m_fps = MathsUtils::Clamp(fps, 0.01f, 1000.0f);
		m_millisecondsPerFrame = 1000.0f/m_fps;
		m_millisecondsPerHalfFrame = 500.0f/m_fps;
	}

	// Set the number of frames ie |-|-|-|-| (where - is a frame and | is a frame sample)
	void SetNumFrames(int numFrames)
	{
		m_numFrames = numFrames;
	}
				
	void SetName(const std::string &name)
	{
		m_name = name;
	}

	void SetStartSample(
		int startSample
		)
	{
		m_startSample = startSample;
	}

	void SetEndSample(
		int endSample
		)
	{
		m_endSample = endSample;
	}

	const double GetFPS() const
	{
		return m_fps;
	}

	const int GetNumFrames() const
	{
		return m_numFrames;
	}

	const int GetNumFrameSamples() const
	{
		return m_numFrames + 1;
	}
				
	const std::string &GetName() const
	{
		return m_name;
	}

	const int GetStartSample() const
	{
		return m_startSample;
	}

	const int GetEndSample() const
	{
		return m_endSample;
	}

	long ConvertFrameToMilliseconds(
		const int frame
	)
	{
		return static_cast<long>(floor(frame * m_millisecondsPerFrame));
	}

	/**
		Set FPS must be called before this
	*/
 int ConvertMillisecondsToFrame(
	const long milliseconds
	)
	{
		return static_cast<int>(floor(milliseconds / m_millisecondsPerFrame));
	}

private:
	// Each frame is 1000 milliseconds apart ie one second
	// Milliseconds per frame = 1000/m_fps; ie If we used seconds it would be 1/m_fps
	// Global time = global start time + ((millisecondsPerFrame/m_fps) * localTime)
	// Local time = (globalTime - localTime) * m_fps
	int m_numFrames;
	std::string m_name;
	int m_startSample;
	int m_endSample;
	float m_fps;
	float m_millisecondsPerFrame;		//1000.0f/m_fps;
	float m_millisecondsPerHalfFrame;	//500.0f/m_fps;
};

